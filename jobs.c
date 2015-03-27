#include "shell.h"

void _jobNull(Job *j) {
    j->pid = 0;
    j->jid = -1;
    j->flags = 0;

    memset(j->cmdName, 0, LINELEN * sizeof(char));
}

void jobsInit(Jobs *jobs) {
    int i = 0;

    for (i=0; i<MAX_JOBS; i++) {
        _jobNull(&(jobs->jobs[i]));
        jobs->exitedPid[i] = -1;
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

int _findInExitedPid(Jobs *jobs, pid_t pid) {
    int i = 0;
    while ((jobs->exitedPid[i] != pid) && (i < MAX_JOBS)) {
        i++;
    }
    if (i != MAX_JOBS) {
        return i;
    }
    return -1;
}

void addExitedPid(Jobs *jobs, pid_t pid) {
    int i = _findInExitedPid(jobs, -1);
    jobs->exitedPid[i] = pid;
    debug(D_JOB, "Added pid{%d} to exitedPid", pid);
}

bool _isPidInExitedPid(Jobs *jobs, pid_t pid) {
    return _findInExitedPid(jobs, pid) != -1;
}

void _deleteExitedPid(Jobs *jobs, pid_t pid) {
    int i = 0;
    if ((i = _findInExitedPid(jobs, pid)) != -1) {
        debug(D_JOB, "Deleted pid{%d} from exitedPid", pid);
        jobs->exitedPid[i] = -1;
    }
}

Job *newJob(Jobs *jobs, pid_t pid, char *cmdName, int flags) {
    if (_isPidInExitedPid(jobs, pid)) {
        debug(D_JOB, "Job with pid{%d} currently exited.", pid);
        if (ISJOBBACKGROUND(flags)) {
            printf("Job [%%%d] with pid{%d} exited\n", jobs->jobsCount + 1, pid);
        }
        _deleteExitedPid(jobs, pid);
        return NULL;
    }
    _findNextEmptyJob(jobs);
    Job *j = &(jobs->jobs[jobs->nextEmpty]);
    j->jid = ++(jobs->jobsCount);
    j->pid = pid;
    j->flags = flags;
    strcpy(j->cmdName, cmdName);
    return j;
}

void _deleteJob(Jobs *jobs, Job *j) {
    if (jobs->nextEmpty > j->jid) {
        jobs->nextEmpty = j->jid;
    }
    j->jid = -1;
    jobs->jobsCount--;
    _deleteExitedPid(jobs, j->pid);
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
        debug(D_JOB, "Job [%d] {%d} exited/killed", j->jid, j->pid);

        if (ISJOBBACKGROUND(flags)) {
            printf("Job [%%%d] with pid {%d} exited\n", j->jid, j->pid);
        }

        _deleteExitedPid(jobs, j->pid);
        _deleteJob(jobs, j);
    }
}

pid_t _waitJob(Jobs *jobs, pid_t pid, int options) {
    /* when job exited, first call updateJobs in signal handler, or
     * wait in run->waitPid
     */

    int flags = 0;
    pid_t retPid = -1;
    int status = 0;
    Job *j = NULL;
    bool pidIsExited = false;

    if (-1 != pid) {
        pidIsExited = _isPidInExitedPid(jobs, pid);
        j = getJobByPid(jobs, pid);

        if (!j) {
            debug(D_JOB, "Job with pid {%d} not found", pid);

            if (pidIsExited) {
                /* this can be only in childDieHandler->updateJobs */
                // If job not found, mark it as exited and return
                _deleteExitedPid(jobs, pid);
                return pid;
            }
        } else {
            flags = j->flags;
            // pidIsExited not handle, because
            // addExitedPid may be only after this
        }
    }

    // if {pid} not exited and Job exist or
    // if {pid} not exited and Job doesn't exits, wait while they change status
    // waitpid returned -1 if error
    // 0 if children doesn't change status
    retPid = waitpid(pid, &status, options);

    if (retPid <= 0) {
        return retPid;
    }

    if (!j) {
        j = getJobByPid(jobs, retPid);
        flags = j ? j->flags : 0;
    }

    debug(D_JOB, "Job with pid {%d} change status", retPid);

    if (WIFEXITED(status) | WIFSIGNALED(status)) {
        addExitedPid(jobs, retPid);
        debug(D_RUN, "Set shell(gid[%d]) to foreground", getpgid(0));
        tcsetpgrp(0, getpgid(0));
        SETJOBFLAG(flags, JOBEND, 1);
    } elif (WIFSTOPPED(status)) {
        SETJOBFLAG(flags, JOBSTOPPED, 1);
        debug(D_RUN, "Set shell(gid[%d]) to foreground", getpgid(0));
        tcsetpgrp(0, getpgid(0));
    } elif (WIFCONTINUED(status)) {
        SETJOBFLAG(flags, JOBSTOPPED, 0);
    }

    if (j) {
        _updateJob(jobs, j, flags);
    }
    return retPid;
}

void waitForegroundJob(Jobs *jobs, pid_t pid) {
    _waitJob(jobs, pid, 0);
}

void updateJobs(Jobs *jobs) {
    debugSimple(D_JOB, "Update jobs status");
    while (0 < (_waitJob(jobs, -1, WNOHANG | WUNTRACED)));
}

