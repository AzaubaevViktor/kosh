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

int runChild(Context *cntx, int i, int pipeOld[2], int pipeNew[2]) {
    Command *cmd = &(cntx->cmds[i]);
    char *cmdName = cmd->cmdargs[0];
    int chPid = 0;
    int err = 0;

    // Forked
    if (0 != (chPid = fork())) {
        return chPid;
    }

    debug(D_RUN, "CHILD forked. PID:{%d} PPID:{%d}", getpid(), getppid());

    // set redirection
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

    execvpe(cmdName, cmd->cmdargs, environ);

    // TODO: Перенаправить вывод обратно в stdin
    printf(ERROR_FORMAT, cmdName, errno, strerror(errno));
    exit(-1);
}

int run(Context *cntx, int i) {
    Command *cmd = &(cntx->cmds[i]);
    char *cmdName = cmd->cmdargs[0];
    BuiltinCmdType *builtinCmd = getCmdByName(cmdName);
    int chPid = 0;
    static int pipeOld[2] = {-1, -1}, pipeNew[2] = {-1, -1};
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

    if (builtinCmd) {
        builtinCmd(cmdName, cmd->cmdargs, environ);
    } else {
        chPid = runChild(cntx, i, pipeOld, pipeNew);
    }

    // Parent
    int status = 0;
    debug(D_RUN,"PARENT: Children PID: {%d}, bckgr:%d", chPid, isBackground(cmd));
    if (!isBackground(cmd)) {
        debug(D_RUN, "Waiting for status from {%d}", chPid);
        waitpid(chPid, &status, 0);
        debug(D_RUN, "PID {%d} closed with status: `%d`", chPid, status);
    }

    return 0;
}

