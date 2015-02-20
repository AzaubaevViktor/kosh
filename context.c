#include <shell.h>
#include <stdio.h>

void contextNull(Context *cntx) {
    int j, i = j = 0;
    for (i = 0; i < MAXCMDS; i++) {
        cntx->cmds[i].cmdflag = 0;
        cntx->cmds[i].infile = cntx->cmds[i].outfile = cntx->cmds[i].appfile = (char *) NULL;
        for (j = 0; j < MAXARGS; j++) {
            cntx->cmds[i].cmdargs[j] = (char *) NULL;
        }
    }
}
