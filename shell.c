#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include "shell.h"

#define _DEBUG

extern int errno;

int main(int argc, char *argv[]) {
    register int i;
    int pid;
    char line[LINELEN];
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

#ifdef DEBUG
            printf("Run `%s`\n",cntx.cmds[i].cmdargs[0]);
#endif
            if ((pid = fork()) > 0) {
                // Parent
                int status = 0;
                int pid_end = wait(&status);
            } else {
                execvp(cntx.cmds[i].cmdargs[0], cntx.cmds[i].cmdargs);
                printf("`%s`: Error %d: `%s`\n", cntx.cmds[i].cmdargs[0], errno, strerror(errno));
            }

        }

    }  /* close while */
    return 0;
}

/* PLACE SIGNAL CODE HERE */
