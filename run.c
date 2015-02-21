#include "shell.h"

extern char **environ;

#define _exec() if (builtinCmd) {\
    builtinCmd(cmdName, cmd->cmdargs, environ);\
    } else {\
    execvp(cmdName, cmd->cmdargs);\
    }\
    printf("`%s`: error #%d '%s'", cmdName, errno, strerror(errno));

int run(Context *cntx, int i) {
    Command *cmd = &(cntx->cmds[i]);
    char *cmdName = cmd->cmdargs[0];
    BuiltinCmdType *builtinCmd = getCmdByName(cmdName);
    int pid, pidBackground;

    if ((pid = fork()) > 0) {
        // Parent
        int status = 0;
        if (!isBackground(cmd)) {
            int pid_end = wait(&status);
        }
    } else {
        // Child
        if (isBackground(cmd)) {
            if ((pidBackground = fork()) > 0) {
                // Child
                int status = 0;
                printf("Job `%d` started\n", pidBackground);
                wait(&status);
                printf("Job (%s) `%d` has ended\n", cmdName, pidBackground);
                exit(0);
            } else {
                // GrandChild -- Background
                _exec()
            }
        } else {
            _exec()
        }
        exit(-1);
    }
    return 0;
}

