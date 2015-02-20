#include <shell.h>
#include <stdio.h>

#define printTest(format, str) printf((format), (str) ? (str) : "----");

void printCommand(Command *cmd) {
    int i = 0;
    printf("[");
    for(i = 0; (cmd->cmdargs[i]) && (i < MAXARGS); i++) {
        if (i != 0) {
            printf(", ");
        }
        printf("'%s'", cmd->cmdargs[i]);
    }
    printf("] PIP:(%s, %s)",
           cmd->cmdflag & INPIP ? "IN " : "---",
           cmd->cmdflag & OUTPIP ? "OUT" : "---");

}

void printContext(Context *cntx) {
    int i = 0;

    printf("Files:\n");
    printTest("In : `%s`\n", cntx->infile);
    printTest("Out: `%s`\n", cntx->outfile);
    printTest("App: `%s`\n", cntx->appfile);
    printf("Background: %s\n", cntx->bkgrnd ? "Yes" : "No");

    for (i = 0; i < cntx->ncmds; i++) {
        printCommand(&cntx->cmds[i]);
        printf("\n");
    }

}
