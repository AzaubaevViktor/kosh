#include "shell.h"

extern volatile sig_atomic_t isChild;

void intHandler(int i)
{
    printf ("Interrupting {%d} Child:%d\n", getpid(), isChild);
    if (isChild) {
        exit(EXIT_SUCCESS);
    }
}

void stopHandler(int i) {
    printf("Stop Signal Catch\n");
}

void signalInit() {
    debug(D_SIGNALS, "Set signalHandler for {%d}", getpid());
    signal(SIGINT, intHandler);
    signal(SIGTSTP, stopHandler);
}
