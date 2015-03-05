#include "shell.h"

extern volatile sig_atomic_t isChild;

void intHandler(int i)
{
    printf ("Interrupting {%d} Child:%d\n", getpid(), isChild);
    if (isChild) {
        exit(EXIT_SUCCESS);
    }
}

void mySignalSet() {
#ifdef D_SIGNALS
    printf("%s Set signal for {%d}\n", D_SIGNALS, getpid());
#endif

    signal(SIGINT, intHandler);
}
