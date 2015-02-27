#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "shell.h"
#include "tokenizer.h"

#define curCmd (cntx->cmds[ncmds])
#define nextCmd (cntx->cmds[ncmds + 1])
#define curArg (curCmd.cmdargs[nargs])
#define nextArg (curCmd.cmdargs[nargs + 1])
#define newCmd() {curArg = (char *) NULL; nargs = 0; ncmds++;}
#define curToken (tLine.tokens[tokenCounter])
#define nextToken (tLine.tokens[tokenCounter + 1])

int parseline(Context *cntx, char *line)
{
    int nargs, ncmds;

    /* initialize  */
    nargs = ncmds = 0;
    int tokenCounter = 0;

    contextNull(cntx);

    TokensLine tLine;
    tokenizer(&tLine, line);
    if (isShellError()) {
        return -1;
    }

#ifdef D_PARSER
    printf("%s\n", D_PARSER);
    printTokensLine(&tLine);
    printf("\n");
#endif
    // Error check

    while(empty != curToken.type) {
        switch (curToken.type) {
        case str:
            curArg = curToken.str;
            nargs++;
            tokenCounter++;
            break;
        case action:
            switch (curToken.action) {
            case conveyor:
                curCmd.cmdflag |= OUTPIP;
                nextCmd.cmdflag |= INPIP;
                newCmd();
                tokenCounter++;
                break;
            case background:
                curCmd.cmdflag |= BACKGROUND;
                newCmd();
                tokenCounter++;
                break;
            case infile:
                curCmd.infile = nextToken.str;
                tokenCounter += 2;
                break;
            case outfile:
                curCmd.outfile = nextToken.str;
                tokenCounter += 2;
                break;
            case appfile:
                curCmd.appfile = nextToken.str;
                tokenCounter += 2;
                break;
            case semicolon:
                newCmd();
                tokenCounter++;
                break;
            case start:
            case end:
                tokenCounter++;
                break;
            default:
                shellErrorRet(1, ParserErr);
                break;
            }
            break;
        default:
            shellErrorRet(1, ParserErr);
            break;
        }
    }
    ncmds += curCmd.cmdargs[0] ? 1 : 0;

    cntx->ncmds = ncmds;
    return ncmds;
}
