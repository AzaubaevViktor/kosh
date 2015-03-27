#include <shell.h>

void _commandNull(Command *cmd) {
    int j = 0;
    cmd->cmdflag = 0;
    cmd->infile = cmd->outfile = cmd->appfile = (char *) NULL;
    for (j = 0; j < MAXARGS; j++) {
        cmd->cmdargs[j] = (char *) NULL;
    }
}

void makeCmdLine(Command *cmd, char *cmdline) {
    int i = 0;
    int len = 0;
    cmdline[0] = '\0';
    while (cmd->cmdargs[i]) {
        sprintf(cmdline + len, "%s", cmd->cmdargs[i]);
        len += strlen(cmd->cmdargs[i]);
        cmdline[len++] = ' ';
        i++;
    }
    cmdline[--len] = '\0';
}

void commandClean(Context *cntx) {
    int i = 0;
    for (i = 0; i < MAXCMDS; i++) {
        _commandNull(&(cntx->cmds[i]));
    }
}

void contextInit(Context *cntx, int argc, char **argv) {
    cntx->ncmds = 0;
    cntx->argc = argc;
    cntx->argv = argv;
    cntx->fromFile = false;
    jobsInit(&(cntx->jobs));

    commandClean(cntx);
}
