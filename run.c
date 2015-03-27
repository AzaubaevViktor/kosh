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

int _runChild(Context *cntx, int i) {
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
            debug(D_PIPE, "Use @%d<-", cmd->pipeIn);
            dup2(cmd->pipeIn, STDIN_FILENO);
            close(cmd->pipeIn);
        }

        if (isOutPip(cmd)) {
            debug(D_PIPE, "Use <-@%d", cmd->pipeOut);
            dup2(cmd->pipeOut, STDOUT_FILENO);
            close(cmd->pipeOut);
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

int run(Context *cntx, int i) {
    static int pipes[2] = {-1, -1};
    Command *cmd = &(cntx->cmds[i]);
    char *cmdName = cmd->cmdargs[0];
    BuiltinCmd *builtinCmd = getCmdByName(cmdName);
    int chPid = 0;

    if (isInPip(cmd)) {
        debug(D_PIPE, "Use @%d<-", pipes[0]);
        cmd->pipeIn = pipes[0];
    } else {
        cmd->pipeIn = -1;
    }

    if (isOutPip(cmd)) {
        pipe2(pipes, 0);
        debug(D_PIPE, "Create @%d<- <-@%d", pipes[0], pipes[1]);
        debug(D_PIPE, "Use <-@%d", pipes[1]);
        cmd->pipeOut = pipes[1];
    } else {
        cmd->pipeOut = -1;
    }

    if (builtinCmd) {
        builtinCmd(cmdName, cmd->cmdargs, environ);
    } else {
        chPid = _runChild(cntx, i);
        if (isBackground(cmd)) {
            debug(D_RUN, "DONT set foreground group {{%d}}, because process "
                         "is background", chPid);
        } else {
            waitForegroundJob(&(cntx->jobs), chPid);
            if (-1 != cmd->pipeOut) {
                debug(D_PIPE, "Close <-@%d", cmd->pipeOut);
                close(cmd->pipeOut);
            }
            if (-1 != cmd->pipeIn) {
                debug(D_PIPE, "Close @%d<-", cmd->pipeIn);
                close(cmd->pipeIn);
            }
        }
    }

    return 0;
}
