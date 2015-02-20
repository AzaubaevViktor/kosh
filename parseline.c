#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "shell.h"
#include "tokenizer.h"

enum ShellErrors shellError;

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

    clearTokensLine(&tLine);

    tokenizer(&tLine, line);

    printTokensLine(&tLine);

    // Error check

    while(empty != curToken.type) {
        switch (curToken.type) {
        case str:
        case quotedStr:
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
            default:
                shellError = InternalErr;
                return -1;
                break;
            }
            break;
        default:
            shellError = InternalErr;
            return -1;
            break;
        }
    }

    cntx->ncmds = ncmds + 1;
    return ncmds + 1;
}
