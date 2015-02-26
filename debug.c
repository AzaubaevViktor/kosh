#include <stdio.h>
#include "shell.h"
#include "tokenizer.h"


#ifdef D_COMMANDS
#define printTest(format, str) printf((format), (str) ? (str) : "---");
void printCommand(Command *cmd) {
    int i = 0;

    printf("  +CMD:\n");
    printTest("  | In : '%s'\n", cmd->infile);
    printTest("  | Out: '%s'\n", cmd->outfile);
    printTest("  | App: '%s'\n", cmd->appfile);

    printf("  | [");
    for(i = 0; (cmd->cmdargs[i]) && (i < MAXARGS); i++) {
        if (i != 0) {
            printf(", ");
        }
        printf("'%s'", cmd->cmdargs[i]);
    }
    printf("] FLAGS:(%s, %s, %s)\n",
           isInPip(cmd) ? "IN " : "---",
           isOutPip(cmd) ? "OUT" : "---",
           isBackground(cmd) ? "BGR" : "---");

}
#undef printTest
#endif


#ifdef D_COMMANDS
void printContext(Context *cntx) {
    int i = 0;
    for (i = 0; i < cntx->ncmds; i++) {
        printCommand(&cntx->cmds[i]);
        printf("  =================================\n");
    }
}
#endif

#ifdef D_TOKENS
void printStrLine(char *line) {
    int i = 0;
    printf("  ");
    for (i = 0; '\n' != line[i]; i++) {
        printf("%2X`%1c|",
               line[i], isprint(line[i]) ? line[i] : ' ');
    }
}
#endif

#ifdef D_PARSER
#define curToken (tLine->tokens[i])
void printTokensLine(TokensLine *tLine) {
    int i = 0;
    static char *typesStr[] = {"∅", "S", "Q", "A"};

    printf("  TOKENS:\n  ");

    while (empty != curToken.type) {
        printf("%s", typesStr[curToken.type]);
        if (action == curToken.type)
            printf("[%d]", curToken.action);
        else
            printf("[%s]", curToken.str);
        printf(" ");
        i++;
    }
}
#undef curToken
#endif
