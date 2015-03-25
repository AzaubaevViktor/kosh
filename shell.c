#include "shell.h"

extern int errno;

extern Context *cntx;

// Конвейер + pipe
// Маска прав по умолчанию?

int main(int argc, char *argv[]) {
    register int i;
    char line[LINELEN];
    Context lCntx;
    cntx = &lCntx;
    lCntx.argv = argv;
    lCntx.argc = argc;
    lCntx.fromFile = false;

    if (2 == argc) {
        int fd = open(argv[1], O_RDONLY);
        dup2(fd, STDIN_FILENO);
        close(fd);
        lCntx.fromFile = true;
    }

        signalInit();
        jobsInit(&(lCntx.jobs));

    while (1) {
        promptline(&lCntx, line);
        /* il eof  */

        if (*line == '\0') {
            continue;
        }

        if (parseline(&lCntx, line) <= 0)
            continue;   /* read next line */

        if (isShellError()) {
            printf("%s\n", getShellError());
            continue;
        }

    debug(D_COMMANDS, "", printContext(&lCntx));

        for (i = 0; i < lCntx.ncmds; i++) {

    debug(D_MAIN,"Run `%s`", lCntx.cmds[i].cmdargs[0]);
            run(&lCntx, i);
            if (isShellError()) {
                printf("%s\n", getShellError());
                continue;
            }
        }

    }  /* close while */
    return 0;
}

/* PLACE SIGNAL CODE HERE */
