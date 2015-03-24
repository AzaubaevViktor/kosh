#include "shell.h"

extern volatile sig_atomic_t isChild;

void intHandler(int i) {
    printf("Interrupt signal catch\n");
}

void stopHandler(int i) {
    printf("Stop Signal Catch\n");
}

extern Context *cntx;

void childAction(int i, siginfo_t *sinfo, void *context) {
    debug(D_SIGNALS, "Child {%d} change state", sinfo->si_pid);
    pid_t pid = sinfo->si_pid;
    int jobStatus = sinfo->si_status;

    Jobs *jobs = &cntx->jobs;
    Job *j = getJobByPid(jobs, pid);

    if (!j) {
        debug(D_JOB, "Job with pid {%d} not found", pid);
        return;
    }

    int flags = j->flags;

    SETJOBFLAG(flags, JOBEND, WIFEXITED(jobStatus));
    SETJOBFLAG(flags, JOBSTOPPED, WIFSTOPPED(jobStatus));

    updateJob(jobs, j, flags);

    debugSimple(D_RUN, "Set shell to foreground");
    tcsetpgrp(0, getpgid(0));

    debug(D_JOB, "Job status is `%d`", jobStatus);
}

void signalInit() {
    debug(D_SIGNALS, "Set signalHandler for {%d}", getpid());
    // TODO: with block
    signal(SIGINT, intHandler);
    signal(SIGTSTP, stopHandler);
    signal(SIGQUIT, SIG_IGN);

    struct sigaction setup_action;
    sigset_t block_mask;

    sigemptyset (&block_mask);
    /* Block other terminal-generated signals while handler runs. */
    sigaddset (&block_mask, SIGINT);
    sigaddset (&block_mask, SIGTSTP);
    sigaddset (&block_mask, SIGCHLD);
    setup_action.sa_sigaction = childAction;
    setup_action.sa_mask = block_mask;
    setup_action.sa_flags = SA_SIGINFO;
    sigaction (SIGCHLD, &setup_action, NULL);
}
