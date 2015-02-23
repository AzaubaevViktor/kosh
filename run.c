#include "shell.h"

extern char **environ;

char *pipGetInt() {
    static char tempName[] = "/tmp/koshXXXXXXXXXXX";
    static char isCreate = 0;
    if (!isCreate) {
        mkstemp(tempName);
        mkfifo(tempName, 0600);
    } else {
        isCreate = 1;
    }
    return tempName;
}

int redirection(char *filename, int flags, int to) {
    int fileDesc = open(filename, flags);
    if (-1 == fileDesc) {
        printf("%s\n", getErrorStr(FileOpenError));
    }
    shellErrorRet(-1 == fileDesc, FileOpenError);
    dup2(fileDesc, to);
    close(fileDesc);
    return 0;
}

int run(Context *cntx, int i) {
    Command *cmd = &(cntx->cmds[i]);
    char *cmdName = cmd->cmdargs[0];
    BuiltinCmdType *builtinCmd = getCmdByName(cmdName);
    int pid = 0, pidBackground = 0;
    static int pipeOld[2] = {-1, -1}, pipeNew[2] = {-1, -1};
    int err = 0;
    int j = 0;

    for (j = 0; j < 2; j++) {
        if (-1 != pipeOld[j]) {
            close(pipeOld[j]);
        }
        pipeOld[j] = pipeNew[j];
    }
    pipe(pipeNew);
    printf("New pipe created: @ %d<-%d @\n", pipeNew[0], pipeNew[1]);

    if ((pid = fork()) > 0) {
        // Parent
        int status = 0;
        //        printf("PARENT: pid1: %d, pidBackgr: %d; bckgr:%d\n", pid, pidBackground, isBackground(cmd));
        if (!isBackground(cmd)) {
            wait(&status);
        }
    } else {
        // Child
        if (isBackground(cmd) && ((pidBackground = fork()) > 0)) {
            // Watcher for background process. Run if isBackground and pidBa < 0
            int status = 0;
            //            printf("WATCHER: pid1: %d, pidBackgr: %d\n", pid, pidBackground);
            printf("Job `%d` started\n", pidBackground);
            wait(&status);
            printf("Job \"%s\" `%d` has ended\n", cmdName, pidBackground);
            exit(0);
        } else {
            // Not background or pidBack = 0
            //            printf("CHILD pid1: %d, pidBackgr: %d\n", pid, pidBackground);

            if (cmd->infile) {
                if ((err = redirection(cmd->infile, O_RDONLY, STDIN_FILENO)) != 0) {
                    exit(EXIT_FAILURE);
                }
            }
            if (cmd->outfile) {
                if ((err = redirection(cmd->outfile, O_WRONLY, STDOUT_FILENO)) != 0) {
                    exit(EXIT_FAILURE);
                }
            }
            if (cmd->appfile) {
                if ((err = redirection(cmd->appfile, O_WRONLY | O_APPEND, STDOUT_FILENO)) != 0) {
                    exit(EXIT_FAILURE);
                }
            }
            if (isInPip(cmd)) {
                printf("%s: IN@ _%d_<-%d @\n", cmdName, pipeOld[0], pipeOld[1]);
                dup2(pipeOld[0], STDIN_FILENO);
            }
            if (isOutPip(cmd)) {
                printf("%s: OUT@ %d<-_%d_ @\n", cmdName, pipeNew[0], pipeNew[1]);
                dup2(pipeNew[1], STDOUT_FILENO);
            }

            if (builtinCmd) {
                builtinCmd(cmdName, cmd->cmdargs, environ);
            } else {
                execvp(cmdName, cmd->cmdargs);
            }
            printf("`%s`: error #%d '%s'", cmdName, errno, strerror(errno));
        }
        exit(-1);
    }
    return 0;
}

