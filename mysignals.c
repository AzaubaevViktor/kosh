#include "shell.h"

extern volatile sig_atomic_t isChild;

void intHandler(int i) {
    printf("Interrupt signal catch\n");
}

void stopHandler(int i) {
    printf("Stop Signal Catch\n");
}

extern Context *cntx;

void childDieHandler(int i) {
    debugSimple(D_SIGNALS, "Child change state");

    updateJobs(&(cntx->jobs));
}

void signalInit() {
    debug(D_SIGNALS, "Set signalHandler for {%d}", getpid());
    // TODO: with block
    signal(SIGINT, intHandler);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);

    struct sigaction setup_action;
    sigset_t block_mask;

    // For children

    sigemptyset (&block_mask);
    sigaddset (&block_mask, SIGINT);
    sigaddset (&block_mask, SIGTSTP);
    sigaddset (&block_mask, SIGCHLD);
    setup_action.sa_handler = childDieHandler;
    setup_action.sa_mask = block_mask;
    setup_action.sa_flags = 0;
    sigaction (SIGCHLD, &setup_action, NULL);
}
