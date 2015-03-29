#include "shell.h"

extern char **environ;
extern Context *cntx;
extern int errno;

int biHelp(char *name, char **args, char **envs) {
    printf("Hello! This is KOrovin SHell.\n");
    printf("My pid:{%d}; my ppid:{%d}\n", getpid(), getppid());
    printf("ARGS:\n");
    char **curargs = args;
    while (*curargs) {
        printf(" %s\n", *curargs);
        curargs++;
    }
    printf("ENVIRONMENTS:\n");
    char **curenv = envs;
    while (*curenv) {
        printf(" %s\n", *curenv);
        curenv++;
    }
    return 0;
}

int biSet(char *name, char **args, char **environ) {
    int argc = 0;
    static char *res_s[1024] = {0};

    for (argc = 0; args[argc]; argc++);

    if (3 == argc) {
        sprintf((char *) res_s, "%s=%s", args[1], args[2]);
        putenv((char *) res_s);
        return 0;
    } elif (2 == argc) {
        sprintf((char *) res_s, "%s=", args[1]);
        putenv((char *) res_s);
        return 0;
    } elif (1 == argc) {
        char **curenv = environ;
        while (*curenv) {
            printf("%s\n", *curenv);
            curenv++;
        }
    } elif (3 < argc) {
        printf("To more arguments\n");
        return -1;
    }
    return 0;
}

int biExit(char *name, char **args, char **environ) {
    printf("\nBye!\n");
    exit(0);
}

int _argParser(char **args) {
    Job *job = NULL;
    char *arg = *args;
    int jid = 0;
    int error = 0;

    if (arg == NULL) {
        return LAST_JOB;
    } elif (*arg == '\0') {
        return LAST_JOB;
    } elif (*arg == '+') {
        return LAST_JOB;
    } elif (*arg == '-') {
        return PRELAST_JOB;
    } else {
        errno = 0;
        jid = strtoumax(arg, NULL, 10);
        if ((0 == jid) && ('0' != *arg)) {
            job = getJobByLine(&(cntx->jobs), arg, &error);
            if ((NOT_FOUND_JOB == error) || (AMBIGOUS_JOB == error)) {
                return error;
            }
            return job ? job->jid : NOT_FOUND_JOB;
        } else {
            return jid;
        }
    }
}

int biFg(char *name, char **args, char **environ) {
    int jid = _argParser(&(args[1]));
    if (NOT_FOUND_JOB == jid) {
        printf("Job `%s` not found\n", args[1]);
        return -1;
    } elif (AMBIGOUS_JOB == jid) {
        printf("'%s': ambigous job spec\n", args[1]);
        return -1;
    }

    Job *job = getJobByJid(&(cntx->jobs), jid);

    setLastOrderJob(&(cntx->jobs), job);
    SETJOBFLAG(job->flags, JOBBACKGROUND, 0);
    printf("Set foreground job [%%%d] '%s'\n", job->jid, job->cmdLine);
    debug(D_RUN, "Set {%d} to foreground", job->pid);
    tcsetpgrp(STDIN_FILENO, job->pid);
    debug(D_SIGNALS, "Send SIGCONT to %d", job->pid);
    kill(job->pid, SIGCONT);
    return job->pid;
}

int biBg(char *name, char **args, char **environ) {
    int jid = _argParser(&(args[1]));
    if (NOT_FOUND_JOB == jid) {
        printf("Job `%s` not found\n", args[1]);
        return -1;
    } elif (AMBIGOUS_JOB == jid) {
        printf("'%s': ambigous job spec\n", args[1]);
        return -1;
    }

    Job *job = getJobByJid(&(cntx->jobs), jid);

    if (!ISJOBSTOPPED(job->flags)) {
        printf("Job [%%%d] '%s' already running\n", job->jid, job->cmdLine);
        return -1;
    }

    printf("Set backround job [%%%d] '%s'\n", job->jid, job->cmdLine);
    debug(D_SIGNALS, "Send SIGCONT to %d", job->pid);
    kill(job->pid, SIGCONT);
    return 0;
}

int biJobs(char *name, char **args, char **environ) {
    Jobs *jobs = &cntx->jobs;
    Job *j = NULL;
    char *s = NULL;

    updateJobs(jobs);

    int i = 0;

    if (jobs->jobsCount == 0) {
        printf("No jobs\n");
    }

    for (i=0; i < MAX_JOBS; i++) {
        j = &jobs->jobs[i];
        switch (j->order) {
        case LAST_JOB:
            s = "+";
            break;
        case PRELAST_JOB:
            s = "-";
            break;
        default:
            s = "";
            break;
        }

        if (-1 != j->jid) {
            printf("[%%%d]%s `%s` {%d} [%s|%s]\n",
                   j->jid,
                   s,
                   j->cmdLine,
                   j->pid,
                   ISJOBBACKGROUND(j->flags) ? "backgr" : "foregr",
                   ISJOBSTOPPED(j->flags) ? "stopped" : "running");
        }
    }

    return 0;
}

int biVoid(char *name, char **args, char **environ) {
    return 0;
}

static struct {
    char *name;
    BuiltinCmd *cmd;
} builtinCommands[] = {
{"help", biHelp},
{"set", biSet},
{"exit", biExit},
{"fg", biFg},
{"bg", biBg},
{"jobs", biJobs},
{NULL, NULL}
};

BuiltinCmd *getCmdByName(char *name) {
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
