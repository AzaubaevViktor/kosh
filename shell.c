#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include "shell.h"

#define DEBUG

int main(int argc, char *argv[]) {
    register int i;
    char line[2048];      /*  allow large command lines  */
    int ncmds;
    char prompt[200];      /* shell prompt */
    Context cntx;

    /* PLACE SIGNAL CODE HERE */

    sprintf(prompt,"[%s]> ", argv[0]);

    while (promptline(prompt, line, sizeof(line)) > 0) {    /* il eof  */
        if ((ncmds = parseline(&cntx, line)) <= 0)
            continue;   /* read next line */
#ifdef DEBUG
        {
            int i, j;
            for (i = 0; i < ncmds; i++) {
                for (j = 0; cntx.cmds[i].cmdargs[j] != (char *) NULL; j++)
                    fprintf(stderr, "cmd[%d].cmdargs[%d] = %s\n",
                            i, j, cntx.cmds[i].cmdargs[j]);
                fprintf(stderr, "cmds[%d].cmdflag = %o\n", i,
                        cntx.cmds[i].cmdflag);
            }
        }
#endif

        for (i = 0; i < ncmds; i++) {

            /*  FORK AND EXECUTE  */

        }

    }  /* close while */
    return 0;
}

/* PLACE SIGNAL CODE HERE */
