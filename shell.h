#ifndef SHELL_H
#define SHELL_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include "shellerrors.h"
#include "builtin.h"
#include "debugutil.h"

#define MAXARGS (256)
#define MAXCMDS (50)
#define LINELEN (2048)

/* strings */
#define ERROR_FORMAT "`%s`: error #%d '%s'\n"
// cmdName, errno, strerror(errno)

/*  cmdflag's  */
#define OUTPIP  (01)
#define INPIP   (02)
#define BACKGROUND (04)

#define isBackground(cmd) (cmd->cmdflag & BACKGROUND)
#define isInPip(cmd) (cmd->cmdflag & INPIP)
#define isOutPip(cmd) (cmd->cmdflag & OUTPIP)

/* debugs */

#define DEBUG_OFF

#define D_MAIN     1
#define D_COMMANDS 1
#define D_SIGNALS  1
#define D_BUILTIN  1
#define D_PARSER   1
#define D_RUN      1
#define D_PIPE     1
#define D_TOKENS   1

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


#ifdef DEBUG_OFF
#define D_MAIN     0
#define D_COMMANDS 0
#define D_SIGNALS  0
#define D_BUILTIN 0
#define D_PARSER  0
#define D_RUN      0
#define D_PIPE     0
#define D_TOKENS  0
#endif


typedef struct _Command {
    char *cmdargs[MAXARGS];
    char cmdflag;
    char *infile, *outfile, *appfile;
} Command;

typedef struct _Context {
    Command cmds[MAXCMDS];
    int ncmds;
    int argc;
    char **argv;
} Context;

/* Debug */
#ifdef D_TOKENS
void printStrLine(char *line);
#endif
#ifdef D_COMMANDS
int printContext(Context *cntx);
void printCommand(Command *cmd);
#endif

int parseline(Context *, char *);
int promptline(Context *, char *, int fromFile);
void contextNull(Context *);
int run(Context *, int);
void signalInit(void);

#endif // SHELL_H

