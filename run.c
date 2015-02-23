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

int run(Context *cntx, int i) {
    Command *cmd = &(cntx->cmds[i]);
    char *cmdName = cmd->cmdargs[0];
    BuiltinCmdType *builtinCmd = getCmdByName(cmdName);
    int pid = 0, pidBackground = 0;
    static int fdPrev[2], fdCur[2];
    int fileDesc;

//    close(fdPrev[0]);
//    close(fdPrev[1]);
    fdPrev[0] = fdCur[0];
    fdPrev[1] = fdCur[1];
    pipe(fdCur);

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
                fileDesc = fileno(fopen(cmd->infile, "r"));
                shellErrorRet(errno, FileOpenError);
                dup2(fileDesc, STDIN_FILENO);
                close(fileDesc);
            }
            if (cmd->outfile) {
                fileDesc = fileno(fopen(cmd->outfile, "w"));
                shellErrorRet(errno, FileOpenError);
                dup2(fileDesc, STDOUT_FILENO);
                close(fileDesc);
            }
            if (cmd->appfile) {
                fileDesc = fileno(fopen(cmd->appfile, "a"));
                shellErrorRet(errno, FileOpenError);
                dup2(fileDesc, STDOUT_FILENO);
                close(fileDesc);
            }
            if (isOutPip(cmd)) {
                printf("IN@ %d:%d @\n", fdCur[0], fdCur[1]);
                dup2(fdCur[1], STDOUT_FILENO);
            }
            if (isInPip(cmd)) {
                printf("OUT@ %d:%d @\n", fdPrev[0], fdPrev[1]);
                dup2(fdPrev[0], STDIN_FILENO);
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

