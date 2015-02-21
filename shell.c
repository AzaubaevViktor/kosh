
#include "shell.h"

#define _DEBUG

extern int errno;
extern char **environ;

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
                printf("%d\n", status);
            } else {
                cmdType *cmd = getCmdByName(cntx.cmds[i].cmdargs[0]);
                printf("!%p!\n", cmd);
                if (cmd) {
                    return cmd(cntx.cmds[i].cmdargs[0], cntx.cmds[i].cmdargs, environ);
                } else {
                    execvp(cntx.cmds[i].cmdargs[0], cntx.cmds[i].cmdargs);
                }
                printf("`%s`: Error %d: `%s`\n", cntx.cmds[i].cmdargs[0], errno, strerror(errno));
                return -1;
            }

        }

    }  /* close while */
    return 0;
}

/* PLACE SIGNAL CODE HERE */
