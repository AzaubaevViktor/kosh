#include <shell.h>
#include <stdio.h>

void contextNull(Context *cntx) {
    int j, i = j = 0;
    cntx->bkgrnd = 0;
    cntx->infile = cntx->outfile = cntx->appfile = (char *) NULL;
    for (i = 0; i < MAXCMDS; i++) {
        cntx->cmds[i].cmdflag = 0;
        for (j = 0; j < MAXARGS; j++) {
            cntx->cmds[i].cmdargs[j] = (char *) NULL;
        }
    }
}
