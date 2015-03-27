#include "shell.h"

extern char **environ;

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

int _runChild(Context *cntx, int i, int pipeIn, int pipeOut) {
    Command *cmd = &(cntx->cmds[i]);
    char *cmdName = cmd->cmdargs[0];
    int chPid = 0;
    int err = 0;

    // Forked
    if (0 != (chPid = fork())) {
        Job *j = newJob(&cntx->jobs, chPid, cmdName,
                        JOBBACKGROUND * isBackground(cmd));
        if (j) {
            debug(D_JOB, "Created new job with jid [%%%d], `%s`", j->jid,
                  j->cmdName);
        }

        return chPid;
    }

    signalReset();

    debug(D_RUN, "CHILD forked. PID:{%d} PPID:{%d}", getpid(), getppid());

    // set redirection
    {
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
            debug(D_PIPE, "Use @%d<-", pipeIn);
            dup2(pipeIn, STDIN_FILENO);
            close(pipeIn);
        }

        if (isOutPip(cmd)) {
            debug(D_PIPE, "Use <-@%d", pipeOut);
            dup2(pipeOut, STDOUT_FILENO);
            close(pipeOut);
        }
    }

    execvpe(cmdName, cmd->cmdargs, environ);

    // TODO: Перенаправить вывод обратно в stdin
    debug(D_RUN, "Exec error. Set foreground group [%d] (parent)",
          getpgid(getppid()));
    tcsetpgrp(0, getpgid(getppid()));
    printf(ERROR_FORMAT, cmdName, errno, strerror(errno));
    exit(EXIT_FAILURE);
}

int _run(Context *cntx, int i, bool closeAllPipes) {
    int j = 0;
    static int pipeOld[2] = {-1, -1}, pipeNew[2] = {-1, -1};
    if (closeAllPipes) {
        for (j = 0; j < 2; j++) {
            close(pipeOld[j]);
            close(pipeNew[j]);
        }
        return 0;
    }

    Command *cmd = &(cntx->cmds[i]);
    char *cmdName = cmd->cmdargs[0];
    BuiltinCmd *builtinCmd = getCmdByName(cmdName);
    int chPid = 0;
    int pipeIn = -1;
    int pipeOut = -1;

    if (isInPip(cmd)) {
        debug(D_PIPE, "Use @%d<-", pipeOld[0]);
        pipeIn = pipeOld[0];
    } else {
        debug(D_PIPE, "Close @%d<-", pipeOld[0]);
        close(pipeOld[0]);
    }
    debug(D_PIPE, "Close <-@%d", pipeOld[1]);
    close(pipeOld[1]);

    if (isOutPip(cmd)) {
        pipe2(pipeNew, 0);
        debug(D_PIPE, "Create @%d<- <-@%d",pipeNew[0], pipeNew[1]);
        pipeOut = pipeNew[1];
    } else {
        pipeNew[0] = pipeNew[1] = -1;
    }

    for (j = 0; j < 2; j++) {
        pipeOld[j] = pipeNew[j];
    }

    if (builtinCmd) {
        builtinCmd(cmdName, cmd->cmdargs, environ);
    } else {
        chPid = _runChild(cntx, i, pipeIn, pipeOut);
        if (isBackground(cmd)) {
            debug(D_RUN, "DONT set foreground group {{%d}}, because process "
                         "is background", chPid);
        } else {
            waitForegroundJob(&(cntx->jobs), chPid);
        }
    }

    return 0;
}

int run(Context *cntx, int i) {
    return _run(cntx, i, false);
}

void commandsEnd() {
    _run(NULL, 0, true);
}

