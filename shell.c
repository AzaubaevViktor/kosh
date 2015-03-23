#include "shell.h"

extern int errno;

volatile sig_atomic_t isChild = false;

// Конвейер + pipe
// Маска прав по умолчанию?

int main(int argc, char *argv[]) {
    register int i;
    char line[LINELEN];
    Context cntx;
    cntx.argv = argv;
    cntx.argc = argc;
    int fromFile = false;

    if (2 == argc) {
        int fd = open(argv[1], O_RDONLY);
        dup2(fd, STDIN_FILENO);
        close(fd);
        fromFile = true;
    }

        signalInit();

    while (promptline(&cntx, line, fromFile) > 0) {
        /* il eof  */
        if (parseline(&cntx, line) <= 0)
            continue;   /* read next line */

        if (isShellError()) {
            printf("%s\n", getShellError());
            continue;
        }

    debug(D_COMMANDS, "", printContext(&cntx));

        for (i = 0; i < cntx.ncmds; i++) {

    debug(D_MAIN,"Run `%s`", cntx.cmds[i].cmdargs[0]);
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
