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
    FILE *gInp = NULL;

    contextInit(&lCntx, argc, argv);

    if (2 == argc) {
        gInp = fopen(argv[1], "rt");
        if (!gInp) {
            printf("Error open file %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        cntx->gInp = gInp;
        cntx->fromFile = true;
    }

    signalInit();

    while (1) {
        updateJobs(&(cntx->jobs));
        printPrompt(cntx);
        readCmds(cntx, line);

        /* if eof  */
        if (*line == '\0') {
            continue;
        }

        if (parseline(cntx, line) <= 0)
            continue;   /* read next line */


        if (isShellError()) {
            printf("%s\n", getShellError());
            continue;
        }

        debug(D_COMMANDS, "", printContext(&lCntx));

        for (i = 0; i < cntx->ncmds; i++) {

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
