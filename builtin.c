#include "shell.h"
#include "builtin.h"

extern char **environ;
extern Context *cntx;

int biHelp(char *name, char **args, char **envs) {
    printf("Hello! This is KOrovin SHell.\n");
    printf("My pid:{%d}; my ppid:{%d}\n", getpid(), getppid());
    printf("ARGS:\n");
    char **curargs = args;
    while (*curargs) {
        printf(" > %s\n", *curargs);
        curargs++;
    }
    printf("ENVIRONMENTS:\n");
    char **curenv = envs;
    while (*curenv) {
        printf(" > %s\n", *curenv);
        curenv++;
    }
    return 0;
}

int biSet(char *name, char **args, char **environ) {
    int argc = 0;
    static char *res_s[1024] = {0};
    for (argc = 0; args[argc]; argc++) {}
    if (3 == argc) {
        sprintf((char *) res_s, "%s=%s", args[1], args[2]);
        putenv((char *) res_s);
        return 0;
    } else if (3 < argc) {
        printf("To more arguments\n");
        return 1;
    } else if (3 > argc) {
        printf("To few arguments\n");
        return 1;
    }
    return 0;
}

int biExit(char *name, char **args, char **environ) {
    exit(0);
}

int biFg(char *name, char **args, char **environ) {
    printf("Not realized\n");
    return 1;
}

int biBg(char *name, char **args, char **environ) {
    printf("Not realized\n");
    return 1;
}

int biJobs(char *name, char **args, char **environ) {
    Jobs *jobs = &cntx->jobs;
    Job *j = NULL;

    updateJobs(jobs, NULL);

    int i = 0;

    if (jobs->jobsCount == 0) {
        printf("No jobs\n");
    }

    for (i=0; i < MAX_JOBS; i++) {

        j = &jobs->jobs[i];
        if (-1 != j->jid) {
            printf("[%%%d] `%s` {%d} [%s|%s]\n",
                   j->jid,
                   j->cmdName,
                   j->pid,
                   ISJOBBACKGROUND(j->flags) ? "backgr" : "foregr",
                   ISJOBSTOPPED(j->flags) ? "stopped" : "running");
        }
    }

    return 0;
}

int biVoid(char *name, char **args, char **environ) {
    return 1;
}

BuiltinCommand
builtinCommands[] = {
    {"help", biHelp},
    {"set", biSet},
    {"exit", biExit},
    {"fg", biFg},
    {"bg", biBg},
    {"jobs", biJobs},
    {NULL, NULL}
};

BuiltinCmdType *getCmdByName(char *name) {
    int i = 0;
    for (i = 0; builtinCommands[i].name; i++) {
        debug(D_BUILTIN, "(\"%s\" == \"%s\")? ", builtinCommands[i].name, name);
        if (strcmp(builtinCommands[i].name, name) == 0) {
            debugSimple(D_BUILTIN,"YES");
            return builtinCommands[i].cmd;
        } else {
            debugSimple(D_BUILTIN,"NO");
        }
    }
    return NULL;
}
