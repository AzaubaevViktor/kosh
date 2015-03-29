#include "shell.h"

extern volatile sig_atomic_t isChild;

void intHandler(int i) {
    printf("Interrupt signal catch\n");
}

extern Context *cntx;

void signalInit() {
    debug(D_SIGNALS, "Set signal handlers for {%d}", getpid());
    // TODO: with block
    signal(SIGINT, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGCHLD, SIG_DFL);
}

void signalReset() {
    debug(D_SIGNALS, "Reset signal handlers for {%d}", getpid());
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGCHLD, SIG_DFL);
}

