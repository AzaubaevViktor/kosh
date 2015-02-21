#include "shell.h"

extern char **environ;

int run(Context *cntx, int i) {
    Command *cmd = &(cntx->cmds[i]);
    char *cmdName = cmd->cmdargs[0];
    BuiltinCmdType *builtinCmd = getCmdByName(cmdName);
    int pid;

    if ((pid = fork()) > 0) {
        // Parent
        int status = 0;
        if (!(cmd->cmdflag & BACKGROUND)) {
            int pid_end = wait(&status);
        }
    } else {
        // Child
        if (builtinCmd) {
            builtinCmd(cmdName, cmd->cmdargs, environ);
        } else {
            execvp(cmdName, cmd->cmdargs);
        }
        printf("`%s`: error #%d '%s'", cmdName, errno, strerror(errno));
        exit(-1);
    }
    return 0;
}
