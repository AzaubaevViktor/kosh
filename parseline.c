#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "shell.h"
#include "tokenizer.h"

/* ========================== PARSER ========================== */

#define curCmd (cntx->cmds[ncmds])
#define nextCmd (cntx->cmds[ncmds + 1])
#define curArg (curCmd.cmdargs[nargs])
#define nextArg (curCmd.cmdargs[nargs + 1])
#define newCmd() {curArg = (char *) NULL; nargs = 0; ncmds++;}

int parseline(Context *cntx, char *line)
{
    int nargs, ncmds;
    register char *s;
    char isAppendFlg = 0;
    int rval;
    register int i;
    static char delim[] = " \t|&<>;\n";
    char quote = '\0';

    /* initialize  */
    nargs = ncmds = rval = 0;
    s = line;

    contextNull(cntx);

    TokensLine tLine;

    clearTokensLine(&tLine);

    tokenizer(&tLine, line);

    printTokensLine(&tLine);


    /*  error check  */

    /*
          *  The only errors that will be checked for are
      *  no command on the right side of a pipe
          *  no command to the left of a pipe is checked above
      */
    if (cntx->cmds[ncmds-1].cmdflag & OUTPIP) {
        if (nargs == 0) {
            fprintf(stderr, "syntax error\n");
            return(-1);
        }
    }

    cntx->ncmds = rval;

    return(rval);
}
