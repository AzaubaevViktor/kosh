#include <shell.h>

void _commandNull(Command *cmd) {
    int j = 0;
    cmd->cmdflag = 0;
    cmd->infile = cmd->outfile = cmd->appfile = (char *) NULL;
    for (j = 0; j < MAXARGS; j++) {
        cmd.cmdargs[j] = (char *) NULL;
    }
}

void contextInit(Context *cntx, int argc, char **argv) {
    int i = 0;

    cntx->ncmds = 0;
    cntx->argc = argc;
    cntx->argv = argv;
    cntx->fromFile = false;
    jobsInit(&(cntx->jobs));

    for (i = 0; i < MAXCMDS; i++) {
        _commandNull(&(cntx->cmds[i]));
    }
}
