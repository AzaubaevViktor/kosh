#include "shell.h"

extern int errno;

volatile sig_atomic_t isChild = false;

int main(int argc, char *argv[]) {
    register int i;
    char line[LINELEN];
    Context cntx;
    cntx.argv = argv;
    cntx.argc = argc;

    mySignalSet();

    while (promptline(&cntx, line, sizeof(line)) > 0) {    /* il eof  */
        if (parseline(&cntx, line) <= 0)
            continue;   /* read next line */
        if (isShellError()) {
            printf("%s\n", getShellError());
            continue;
        }

#ifdef D_COMMANDS
        printf("%s ", D_COMMANDS);
        printContext(&cntx);
#endif

        for (i = 0; i < cntx.ncmds; i++) {

#ifdef D_MAIN
            printf("%s Run `%s`\n", D_MAIN, cntx.cmds[i].cmdargs[0]);
#endif
            run(&cntx, i);
            if (isShellError()) {
                printf("%s\n", getShellError());
                continue;
            }
        }

    }  /* close while */
    return 0;
}

/* PLACE SIGNAL CODE HERE */
