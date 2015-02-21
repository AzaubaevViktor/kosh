#include "shell.h"

#define _DEBUG

extern int errno;

int main(int argc, char *argv[]) {
    register int i;
    char line[LINELEN];
    Context cntx;
    cntx.argv = argv;
    cntx.argc = argc;

    /* PLACE SIGNAL CODE HERE */

    while (promptline(&cntx, line, sizeof(line)) > 0) {    /* il eof  */
        if (parseline(&cntx, line) <= 0)
            continue;   /* read next line */
        if (isShellError()) {
            printf(getShellError());
            continue;
        }

#ifdef DEBUG
        printContext(&cntx);
#endif

        for (i = 0; i < cntx.ncmds; i++) {

#ifdef DEBUG
            printf("Run `%s`\n",cntx.cmds[i].cmdargs[0]);
#endif
            run(&cntx, i);
        }

    }  /* close while */
    return 0;
}

/* PLACE SIGNAL CODE HERE */
