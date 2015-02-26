#include "shell.h"

extern volatile sig_atomic_t isChild;

void intHandler(int i)
{
    printf ("Interrupting %d Child:%d\n", getpid(), isChild);
    if (isChild) {
        exit(EXIT_SUCCESS);
    }
}

void mySignalSet() {
    sigset_t newset;

#ifdef DEBUG_SIGNALS
    printf("%s Start set signal for `%d`", DEBUG_SIGNALS, getpid());

#endif

    sigemptyset(&newset);
    sigaddset(&newset, SIGHUP);
    sigprocmask(SIG_BLOCK, &newset, 0);

    signal(SIGINT, intHandler);
}
