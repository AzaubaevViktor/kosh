#include "shell.h"

void _jobNull(Job *j) {
    j->pgid = 0;
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

int _findNextEmptyJob(Jobs *jobs) {
    int i = 0;
    while (jobs->jobs[i].jid == -1) {
        i++;
    }
    return i;
}

Job *newJob(Jobs *jobs, pid_t pid, pid_t pgid, int flags) {
    Job *j = &(jobs->jobs[jobs->nextEmpty]);
    _findNextEmptyJob(jobs);
    j->jid = ++(jobs->jobsCount);
    j->pid = pid;
    j->pgid = pgid;
    j->flags = flags;
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

void updateJob(Jobs *jobs, Job *j, int flags) {
    j->flags = flags;
    if (ISJOBEND(flags)) {
        debug(D_JOB, "Job [%d] {%d} exited/killed", j->jid, j->pid);
        _deleteJob(jobs, j);
    }
}

void updateJobs(Jobs *jobs) {
    siginfo_t sinfo;
    pid_t pid = -1;
    Job *j = NULL;
    int flags = 0;
    debugSimple(D_JOB, "Update jobs status");
    while (-1 != waitid(P_ALL, -1, &sinfo, WNOHANG)) {
        pid = sinfo.si_pid;
        debug(D_JOB, "Job with pid {%d} change status", pid);
        j = getJobByPid(jobs, pid);
        if (!j) {
            debug(D_JOB, "Job with pid {%d} not found", pid);
            return;
        }
        flags = j->flags;
        SETJOBFLAG(flags, JOBEND,
                   sinfo.si_code == CLD_EXITED || sinfo.si_code == CLD_KILLED);
        if (sinfo.si_code == CLD_STOPPED) {
            SETJOBFLAG(flags, JOBSTOPPED, 1);
        } elif (sinfo.si_code == CLD_CONTINUED) {
            SETJOBFLAG(flags, JOBSTOPPED, 0);
        }

        updateJob(jobs, j, flags);
    }
}

