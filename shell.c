#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include "shell.h"

#define DEBUG

int main(int argc, char *argv[]) {
    register int i;
    char line[2048];      /*  allow large command lines  */
    Context cntx;
    cntx.argv = argv;
    cntx.argc = argc;

    /* PLACE SIGNAL CODE HERE */

    while (promptline(&cntx, line, sizeof(line)) > 0) {    /* il eof  */
        if (parseline(&cntx, line) <= 0)
            continue;   /* read next line */

#ifdef DEBUG
        printContext(&cntx);
#endif

        for (i = 0; i < cntx.ncmds; i++) {

            /*  FORK AND EXECUTE  */

        }

    }  /* close while */
    return 0;
}

/* PLACE SIGNAL CODE HERE */
