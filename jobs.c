#include "shell.h"

void _jobNull(Job *j) {
    j->pid = 0;
    j->jid = -1;
    j->flags = 0;
    j->order = 0;
    j->pipeIn = -1;
    j->pipeOut = -1;
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

void addOrderJob(Jobs *jobs, Job *job) {
    Job *j = NULL;
    int i = 0;

    for (i = 0; i < MAX_JOBS; i++) {
        j = &(jobs->jobs[i]);
        if ((j != job) && (0 != j->order)) {
            j->order--;
        }
    }

    job->order = LAST_JOB;
}

void setLastOrderJob(Jobs *jobs, Job *job) {
    Job *j = NULL;
    int i = 0;

    for (i = 0; i < MAX_JOBS; i++) {
        j = &(jobs->jobs[i]);
        if ((0 != j->order) && (j->order > job->order)) {
            j->order--;
        }
    }

    job->order = LAST_JOB;
}

void deleteOrderJob(Jobs *jobs, Job *job) {
    Job *j = NULL;
    int i = 0;

    for (i = 0; i < MAX_JOBS; i++) {
        j = &(jobs->jobs[i]);
        if ((0 != j->order) && (j->order < job->order)) {
            j->order++;
        }
    }
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
            exit(EXIT_FAILURE);
        }
    }
    jobs->jobsCount++;

    Job *j = &(jobs->jobs[jobs->nextEmpty]);
    j->jid = jobs->nextEmpty + 1;
    _findNextEmptyJob(jobs);
    j->pid = pid;
    j->flags = flags;
    j->pipeIn = cmd->pipeIn;
    j->pipeOut = cmd->pipeOut;
    addOrderJob(jobs, j);
    makeCmdLine(cmd, j->cmdLine);


    return j;
}

void _deleteJob(Jobs *jobs, Job *j) {
    if (jobs->nextEmpty > (j->jid - 1)) {
        jobs->nextEmpty = j->jid - 1;
    }
    deleteOrderJob(jobs, j);
    j->jid = -1;
    jobs->jobsCount--;
}

Job *getJobByJid(Jobs *jobs, int jid) {
    int i = 0;
    if (jid < 0) {
        while((jobs->jobs[i].order != jid) && (i < MAX_JOBS)) {
            i++;
        }
    } else {
        i = jid - 1;
    }
    if (i == MAX_JOBS) {
        return NULL;
    }
    return &(jobs->jobs[i]);
}

Job *getJobByPid(Jobs *jobs, int pid) {
    int i = 0;
    while ((jobs->jobs[i].pid != pid) && (i < MAX_JOBS)) {
        i++;
    }
    if (i == MAX_JOBS) {
        return NULL;
    }
    return &(jobs->jobs[i]);
}

Job *getJobByLine(Jobs *jobs, char *line, int *error) {
    Job *job = NULL;
    Job *fJob = NULL;
    int counter = 0;
    int i = 0;

    if (!line) {
        return NULL;
    }

    for (i = 0; (i < MAX_JOBS) && (counter < 2); i++) {
        job = &(jobs->jobs[i]);
        if (-1 == job->jid) {
            job = NULL;
            continue;
        }

        if (job->cmdLine != strstr(job->cmdLine, line)) {
            job = NULL;
            continue;
        } else {
            counter++;
            fJob = job;
            continue;
        }
    }

    if (counter == 0) {
        *error = NOT_FOUND_JOB;
    } elif (counter == 1) {
        return fJob;
    } else {
        *error = AMBIGOUS_JOB;
    }

    return NULL;
}

void _updateJob(Jobs *jobs, Job *j, int flags) {
    j->flags = flags;

    if (ISJOBEND(flags)) {
        if (ISJOBBACKGROUND(flags)) {
            printf("Job [%%%d] '%s' with pid {%d} exited\n", j->jid, j->cmdLine, j->pid);
        }

        if (-1 != j->pipeOut) {
            debug(D_PIPE, "Close <-@%d", j->pipeOut);
            close(j->pipeOut);
        }
        if (-1 != j->pipeIn) {
            debug(D_PIPE, "Close @%d<-", j->pipeIn);
            close(j->pipeIn);
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
        if (!ISJOBBACKGROUND(j->flags) && !ISJOBCONVEYOR(j->flags)) {
            debug(D_RUN, "Set shell(gid{{%d}}) to foreground", getpgid(0));
            tcsetpgrp(0, getpgid(0));
        }
        SETJOBFLAG(flags, JOBEND, 1);
    } elif (WIFSTOPPED(status)) {
        debug(D_JOB, "Job [%%%d] {%d} stopped", j->jid, j->pid);
        fprintf(stderr, "Job [%%%d] {%d} stopped\n", j->jid, j->pid);
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
    _waitJob(jobs, pid, WUNTRACED | WCONTINUED);
}

void updateJobs(Jobs *jobs) {
    debugSimple(D_JOB, "Update jobs status");
    while (0 < (_waitJob(jobs, -1, WNOHANG | WUNTRACED | WCONTINUED)));
}

