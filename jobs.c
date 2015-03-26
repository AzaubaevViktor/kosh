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

void addExitedPid(Jobs *jobs, pid_t pid) {
    int i = 0;
    while ((jobs->exitedPid[i] != -1) && (i < MAX_JOBS)) {
        i++;
    }
    jobs->exitedPid[i] = pid;
    debug(D_JOB, "Added pid{%d} to exitedPid", pid);
}

int _isPidInExitedPid(Jobs *jobs, pid_t pid) {
    int i = 0;
    while ((jobs->exitedPid[i] != pid) && (i < MAX_JOBS)) {
        i++;
    }
    if (i != MAX_JOBS) {
        return i;
    }
    return -1;
}

void _deleteExitedPid(Jobs *jobs, pid_t pid) {
    int i = 0;
    while ((jobs->exitedPid[i] != pid) && (i < MAX_JOBS)) {
        i++;
    }
    if (i != MAX_JOBS) {
        debug(D_JOB, "Deleted pid{%d} from exitedPid", pid);
        jobs->exitedPid[i] = -1;
    }
}

Job *newJob(Jobs *jobs, pid_t pid, char *cmdName, int flags) {
    if (_isPidInExitedPid(jobs, pid) != -1) {
        debug(D_JOB, "Job with pid{%d} currently exited.", pid);
        if (ISJOBBACKGROUND(flags)) {
            printf("Job pid{%d} exited\n", pid);
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

void _updateJob(Jobs *jobs, Job *j, int flags, int *needPrintPrompt) {
    j->flags = flags;
    if (ISJOBEND(flags)) {
        debug(D_JOB, "Job [%d] {%d} exited/killed", j->jid, j->pid);
        if (j && ISJOBBACKGROUND(j->flags)) {
            printf("Process with pid {%d} exited\n", j->pid);
        } else {
            if (needPrintPrompt != NULL) {
                *needPrintPrompt = 1;
            }
        }
        _deleteJob(jobs, j);
    }
}

void updateJobs(Jobs *jobs, int *needPrintPrompt) {
    int status;
    pid_t pid = -1;
    Job *j = NULL;
    int flags = 0;
    if (needPrintPrompt) {
        *needPrintPrompt = 0;
    }

    debugSimple(D_JOB, "Update jobs status");
    // waitpid returned -1 if error
    // 0 if children doesn't change status
    while (0 < (pid = waitpid(-1, &status, WNOHANG))) {
        debug(D_JOB, "Job with pid {%d} change status", pid);
        j = getJobByPid(jobs, pid);
        if (!j) {
            debug(D_JOB, "Job with pid {%d} not found", pid);
            return;
        }

        flags = j->flags;
        if (WIFEXITED(status)) {
            addExitedPid(jobs, pid);
            SETJOBFLAG(flags, JOBEND, 1);
        } elif (WIFSTOPPED(status)) {
            SETJOBFLAG(flags, JOBSTOPPED, 1);
        } elif (WIFCONTINUED(status)) {
            SETJOBFLAG(flags, JOBSTOPPED, 0);
        }

        _updateJob(jobs, j, flags, needPrintPrompt);
    }

    // Find exited jobs


}

