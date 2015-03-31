#ifndef SHELL_H
#define SHELL_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <errno.h>
#define __USE_GNU
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "shellerrors.h"
#include "debugutil.h"

#define elif else if

#define MAXARGS (256)
#define MAXCMDS (50)
#define LINELEN (2048)

/* strings */
#define ERROR_FORMAT "`%s`: error #%d '%s'\n"
// cmdName, errno, strerror(errno)

/*  cmdflag's  */
#define OUTPIP  (1)
#define INPIP   (2)
#define BACKGROUND (4)

#define isBackground(cmd) (!!(cmd->cmdflag & BACKGROUND))
#define isInPip(cmd) (!!(cmd->cmdflag & INPIP))
#define isOutPip(cmd) (!!(cmd->cmdflag & OUTPIP))

/* debugs */

#define _DEBUG_OFF

#define _D_MAIN     1
#define _D_COMMANDS 1
#define D_SIGNALS  1
#define _D_BUILTIN  1
#define _D_PARSER   1
#define D_RUN      1
#define D_PIPE     1
#define _D_TOKENS   1
#define D_JOB      1

#ifndef D_MAIN
#define D_MAIN 0
#endif

#ifndef D_COMMANDS
#define D_COMMANDS 0
#endif

#ifndef D_SIGNALS
#define D_SIGNALS 0
#endif

#ifndef D_BUILTIN
#define D_BUILTIN 0
#endif

#ifndef D_PARSER
#define D_PARSER 0
#endif

#ifndef D_RUN
#define D_RUN 0
#endif

#ifndef D_PIPE
#define D_PIPE 0
#endif

#ifndef D_TOKENS
#define D_TOKENS 0
#endif

#ifndef D_JOB
#define D_JOB 0
#endif


#ifdef DEBUG_OFF
#undef D_MAIN
#undef D_COMMANDS
#undef D_SIGNALS
#undef D_BUILTIN
#undef D_PARSER
#undef D_RUN
#undef D_PIPE
#undef D_TOKENS
#undef D_JOB

#define D_MAIN     0
#define D_COMMANDS 0
#define D_SIGNALS  0
#define D_BUILTIN  0
#define D_PARSER   0
#define D_RUN      0
#define D_PIPE     0
#define D_TOKENS   0
#define D_JOB      0
#endif

/* Jobs */

#define MAX_JOBS (1000)

#define JOBSTOPPED (1)
#define JOBBACKGROUND (2)
#define JOBEND (4)
#define JOBCONVEYOR (8)

#define ISFLAG(flags, flag) (!!(flags & flag))

#define ISJOBSTOPPED(flags) ISFLAG(flags, JOBSTOPPED)
#define ISJOBBACKGROUND(flags) ISFLAG(flags, JOBBACKGROUND)
#define ISJOBEND(flags) ISFLAG(flags, JOBEND)
#define ISJOBCONVEYOR(flags) ISFLAG(flags, JOBCONVEYOR)

#define SETJOBFLAG(flags, flag, value) \
    (flags = ((flags - ISFLAG(flags, flag) * flag) + (!!value) * flag))

#define LAST_JOB (-1)
#define PRELAST_JOB (-2)

#define NOT_FOUND_JOB (-3)
#define AMBIGOUS_JOB (-4)

typedef struct _Job {
    int jid;
    pid_t pid;
    int flags;
    char cmdLine[LINELEN];
    int order;
    int pipeIn, pipeOut;
} Job;

typedef struct _Jobs {
    int nextEmpty;
    int jobsCount;
    Job jobs[MAX_JOBS];
} Jobs;

typedef struct _Command Command;

void jobsInit(Jobs *jobs);
Job *newJob(Jobs *jobs, pid_t pid, Command *cmd, int flags);
Job *getJobByJid(Jobs *jobs, int jid);
Job *getJobByPid(Jobs *jobs, int pid);
Job *getJobByLine(Jobs *jobs, char *line, int *error);
void waitForegroundJob(Jobs *jobs, pid_t pid);
void updateJobs(Jobs *jobs);
void addOrderJob(Jobs *jobs, Job *job);
void setLastOrderJob(Jobs *jobs, Job *job);
void deleteOrderJob(Jobs *jobs, Job *job);

/* Commands */

typedef struct _Command {
    char *cmdargs[MAXARGS];
    char cmdflag;
    char *infile, *outfile, *appfile;
    int pipeIn, pipeOut;
} Command;

typedef struct _Context {
    Command cmds[MAXCMDS];
    int ncmds;
    int argc;
    char **argv;
    Jobs jobs;
    FILE *gInp;
    bool fromFile;
} Context;

Context *cntx;

void commandClean(Context *cntx);
void contextInit(Context *, int argc, char **argv);
void makeCmdLine(Command *cmd, char *line);

/* Builtin commands */

typedef int (BuiltinCmd)(char *, char **, char **);

BuiltinCmd *getCmdByName(char *name);

/* Debug */
#if 1 == D_TOKENS
void printStrLine(char *line);
#endif
#if 1 == D_COMMANDS
int printContext(Context *cntx);
void printCommand(Command *cmd);
#endif

/* Shell */

int parseline(Context *, char *);
void printPrompt(Context *cntx);
void readCmds(Context *cntx, char *line);
int run(Context *, int);

/* Signals */

void signalInit(void);
void signalReset(void);

#endif // SHELL_H

