#include "shell.h"

extern char **environ;
extern volatile sig_atomic_t isChild;

int redirection(char *filename, int flags, int to) {
    int fileDesc = open(filename, flags, 0644);
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
    int chPid = 0, pidBackground = 0;
    static int pipeOld[2] = {-1, -1}, pipeNew[2] = {-1, -1};
    int err = 0;
    int j = 0;

    for (j = 0; j < 2; j++) {
        if (-1 != pipeOld[j]) {
            close(pipeOld[j]);
        }
        pipeOld[j] = pipeNew[j];
    }

    if (isInPip(cmd) || isOutPip(cmd)) {
        pipe2(pipeNew, 0);
        debug(D_RUN,"New pipe created: @ %d<-%d @", pipeNew[0], pipeNew[1]);
    }

    if ((chPid = fork()) > 0) {
        // Parent
        int status = 0;
        debug(D_RUN,"PARENT: pid1: %d, bckgr:%d", chPid, isBackground(cmd));
        if (!isBackground(cmd)) {
            debug(D_RUN, "Waiting for status from {%d}", chPid);

            waitpid(chPid, &status, 0);
            debug(D_RUN, "PID {%d} closed with status: `%d`", chPid, status);
        }
    } else {
        // Child
        debug(D_RUN, "CHILD forked. PID:{%d} PPID:{%d}", getpid(), getppid());
        if (isBackground(cmd) && ((pidBackground = fork()) > 0)) {
            // Watcher for background process.
            // Run if isBackground and pidBa < 0
            int status = 0;
            debug(D_RUN, "WATCHER: pid1: %d, pidBackgr: %d",
                  chPid, pidBackground);
            printf("Job `%d` started\n", pidBackground);
            waitpid(pidBackground, &status, 0);
            printf("Job \"%s\" `%d` has ended\n",
                   cmdName, pidBackground);
            exit(0);
        } else {
            // Not background or pidBack = 0
            isChild = true;

            signalInit();

            debug(D_RUN, "CHILD pid1: %d, pidBackgr: %d",
                  chPid, pidBackground);

            if (cmd->infile) {
                if ((err = redirection(cmd->infile,
                                       O_RDONLY,
                                       STDIN_FILENO)) != 0) {
                    exit(EXIT_FAILURE);
                }
            }
            if (cmd->outfile) {
                if ((err = redirection(cmd->outfile,
                                       O_WRONLY | O_CREAT | O_TRUNC,
                                       STDOUT_FILENO)) != 0) {
                    exit(EXIT_FAILURE);
                }
            }
            if (cmd->appfile) {
                if ((err = redirection(cmd->appfile,
                                       O_WRONLY | O_CREAT | O_APPEND,
                                       STDOUT_FILENO)) != 0) {
                    exit(EXIT_FAILURE);
                }
            }
            if (isInPip(cmd)) {
                debug(D_PIPE,"for `%s`: IN@ _%d_<-%d @",
                      cmdName, pipeOld[0], pipeOld[1]);
                close(pipeOld[1]);
                dup2(pipeOld[0], STDIN_FILENO);
                close(pipeOld[0]);
            }
            if (isOutPip(cmd)) {
                debug(D_PIPE, "for `%s`: OUT@ %d<-_%d_ @",
                      cmdName, pipeNew[0], pipeNew[1]);
                close(pipeNew[0]);
                dup2(pipeNew[1], STDOUT_FILENO);
                close(pipeNew[1]);
            }

            if (builtinCmd) {
                builtinCmd(cmdName, cmd->cmdargs, environ);
            } else {
                execvpe(cmdName, cmd->cmdargs, environ);
            }
            // TODO: Перенаправить вывод обратно в stdin
            printf(ERROR_FORMAT, cmdName, errno, strerror(errno));
        }
        exit(-1);
    }
    return 0;
}

