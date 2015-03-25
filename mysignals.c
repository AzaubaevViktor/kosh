#include "shell.h"

extern volatile sig_atomic_t isChild;

void intHandler(int i) {
    printf("Interrupt signal catch\n");
}

void stopHandler(int i) {
    printf("Stop Signal Catch\n");
}

extern Context *cntx;

void childHandler(int i) {
    int status = 0;

    debugSimple(D_SIGNALS, "Child change state");

    pid_t pid = wait(&status);
    if (pid != -1) {
        Job *j = getJobByPid(&(cntx->jobs), pid);
        if (j && ISJOBBACKGROUND(j->flags)) {
            printf("Process with pid {%d} exited\n", pid);
        } else {
            printMake(cntx);
        }
        debug(D_RUN, "Child exited with status `%d`", status);
    }

    debug(D_RUN, "Set shell(gid[%d]) to foreground", getpgid(0));
    tcsetpgrp(0, getpgid(0));
}

void signalInit() {
    debug(D_SIGNALS, "Set signalHandler for {%d}", getpid());
    // TODO: with block
    signal(SIGINT, intHandler);
    signal(SIGTSTP, stopHandler);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);

    struct sigaction setup_action;
    sigset_t block_mask;

    // For children

    sigemptyset (&block_mask);
    sigaddset (&block_mask, SIGINT);
    sigaddset (&block_mask, SIGTSTP);
    sigaddset (&block_mask, SIGCHLD);
    setup_action.sa_handler = childHandler;
    setup_action.sa_mask = block_mask;
    setup_action.sa_flags = SA_NOCLDSTOP;
    sigaction (SIGCHLD, &setup_action, NULL);
}
