#include "shell.h"

#include <assert.h>

extern int errno;

extern Context *cntx;

// Конвейер + pipe

int test(void) {
    int flags = 0;

    SETJOBFLAG(flags, JOBSTOPPED, 1);
    assert(ISJOBSTOPPED(flags) == 1);
    assert(ISJOBBACKGROUND(flags) == 0);
    assert(ISJOBEND(flags) == 0);

    SETJOBFLAG(flags, JOBSTOPPED, 0);
    assert(ISJOBSTOPPED(flags) == 0);
    assert(ISJOBBACKGROUND(flags) == 0);
    assert(ISJOBEND(flags) == 0);

    SETJOBFLAG(flags, JOBEND, 1);
    SETJOBFLAG(flags, JOBBACKGROUND, 1);
    assert(ISJOBSTOPPED(flags) == 0);
    assert(ISJOBBACKGROUND(flags) == 1);
    assert(ISJOBEND(flags) == 1);

    SETJOBFLAG(flags, JOBBACKGROUND, 0);
    assert(ISJOBSTOPPED(flags) == 0);
    assert(ISJOBBACKGROUND(flags) == 0);
    assert(ISJOBEND(flags) == 1);

}

int main(int argc, char *argv[]) {
    register int i;
    char line[LINELEN];
    Context lCntx;
    cntx = &lCntx;

    contextInit(&lCntx, argc, argv);

    if (2 == argc) {
        int fd = open(argv[1], O_RDONLY);
        dup2(fd, STDIN_FILENO);
        close(fd);
        lCntx.fromFile = true;
    }

    signalInit();

    while (1) {
        updateJobs(&(cntx->jobs));
        printPrompt(cntx);
        readCmds(&lCntx, line);

        /* if eof  */
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
