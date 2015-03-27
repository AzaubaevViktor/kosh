#include "shell.h"

void _jobNull(Job *j) {
    j->pid = 0;
    j->jid = -1;
    j->flags = 0;
}

void jobsInit(Jobs *jobs) {
    int i = 0;

    for (i=0; i<MAX_JOBS; i++) {
        _jobNull(&(jobs->jobs[i]));
    }
    jobs->nextEmpty = 0;
    jobs->jobsCount = 0;
}

void _findNextEmptyJob(Jobs *jobs) {
    int i = 0;
    while (jobs->jobs[i].jid != -1) {
        i++;
    }
    jobs->nextEmpty = i;
}

Job *newJob(Jobs *jobs, pid_t pid, Command *cmd, int flags) {
    debug(D_JOB, "Create new group with PGID {%d}", pid);
    if (setpgid(pid, pid) == -1) {
        perror("Cannot create new group");
        exit(1);
    }

    if (!ISJOBBACKGROUND(flags)) {
        debug(D_JOB, "Set foreground group {%d}", pid);
        if (tcsetpgrp(STDIN_FILENO, pid) == -1) {
            perror("Cannot set foreground group");
            exit(1);
        }
    }

    _findNextEmptyJob(jobs);
    Job *j = &(jobs->jobs[jobs->nextEmpty]);
    j->jid = ++(jobs->jobsCount);
    j->pid = pid;
    j->flags = flags;
    makeCmdLine(cmd, j->cmdLine);
    return j;
}

void _deleteJob(Jobs *jobs, Job *j) {
    if (jobs->nextEmpty > j->jid) {
        jobs->nextEmpty = j->jid;
    }
    j->jid = -1;
    jobs->jobsCount--;
}

Job *getJobByJid(Jobs *jobs, int jid) {
    int i = 0;
    while(jobs->jobs[i].jid != jid) {
        i++;
    }
    return &(jobs->jobs[i]);
}

Job *getJobByPid(Jobs *jobs, int pid) {
    int i = 0;
    while(jobs->jobs[i].pid != pid) {
        i++;
        if (MAX_JOBS == i) {
            return NULL;
        }
    }
    return &(jobs->jobs[i]);
}

void _updateJob(Jobs *jobs, Job *j, int flags) {
    j->flags = flags;

    if (ISJOBEND(flags)) {
        if (ISJOBBACKGROUND(flags)) {
            printf("Job [%%%d] with pid {%d} exited\n", j->jid, j->pid);
        }

        _deleteJob(jobs, j);
    }
}

pid_t _waitJob(Jobs *jobs, pid_t pid, int options) {
    int flags = 0;
    pid_t retPid = -1;
    int status = 0;
    Job *j = NULL;

    debug(D_JOB, "Wait process with pid {%d}", pid);

    if (-1 != pid) {
        j = getJobByPid(jobs, pid);

        flags = j->flags;
    }

    // waitpid returned -1 if error
    // 0 if children doesn't change status
    retPid = waitpid(pid, &status, options);

    debug(D_JOB, "Waitpid returned `%d`", retPid);

    if (retPid <= 0) {
        return retPid;
    }

    if (!j) {
        j = getJobByPid(jobs, retPid);
        flags = j ? j->flags : 0;
    }

    debug(D_JOB, "Job with pid {%d} change status to `%d`", retPid, status);

    if (WIFEXITED(status) | WIFSIGNALED(status)) {
        debug(D_JOB, "Job [%%%d] {%d} exited/killed", j->jid, j->pid);
        debug(D_RUN, "Set shell(gid{{%d}}) to foreground", getpgid(0));
        tcsetpgrp(0, getpgid(0));
        SETJOBFLAG(flags, JOBEND, 1);
    } elif (WIFSTOPPED(status)) {
        debug(D_JOB, "Job [%%%d] {%d} stopped", j->jid, j->pid);
        SETJOBFLAG(flags, JOBSTOPPED, 1);
        SETJOBFLAG(flags, JOBBACKGROUND, 1);
        debug(D_RUN, "Set shell(gid{{%d}}) to foreground", getpgid(0));
        tcsetpgrp(0, getpgid(0));
    } elif (WIFCONTINUED(status)) {
        debug(D_JOB, "Job [%%%d] {%d} continued", j->jid, j->pid);
        SETJOBFLAG(flags, JOBSTOPPED, 0);
    }

    if (j) {
        _updateJob(jobs, j, flags);
    }
    return retPid;
}

void waitForegroundJob(Jobs *jobs, pid_t pid) {
    _waitJob(jobs, pid, WUNTRACED);
}

void updateJobs(Jobs *jobs) {
    debugSimple(D_JOB, "Update jobs status");
    while (0 < (_waitJob(jobs, -1, WNOHANG | WUNTRACED | WCONTINUED)));
}

