#include <shell.h>
#include <stdio.h>

#define printTest(format, str) printf((format), (str) ? (str) : "---");

void printCommand(Command *cmd) {
    int i = 0;

    printf("CMD:\n");
    printTest(" In : '%s'\n", cmd->infile);
    printTest(" Out: '%s'\n", cmd->outfile);
    printTest(" App: '%s'\n", cmd->appfile);

    printf(" [");
    for(i = 0; (cmd->cmdargs[i]) && (i < MAXARGS); i++) {
        if (i != 0) {
            printf(", ");
        }
        printf("'%s'", cmd->cmdargs[i]);
    }
    printf("] FLAGS:(%s, %s, %s)",
           cmd->cmdflag & INPIP ? "IN " : "---",
           cmd->cmdflag & OUTPIP ? "OUT" : "---",
           cmd->cmdflag & BACKGROUND ? "BGR" : "---");

}

void printContext(Context *cntx) {
    int i = 0;

    printf("COMMANDS:\n[");
    for (i = 0; i < cntx->ncmds; i++) {
        printCommand(&cntx->cmds[i]);
        printf("\n");
    }

}
