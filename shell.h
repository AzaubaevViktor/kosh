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

#define _DEBUG_OFF

#define D_MAIN " !Main:"
#define _D_COMMANDS " !Commands:"
#define D_SIGNALS " !Signals:"
#define _D_BUILTIN " !Builtin:"
#define _D_PARSER " !Parser:"
#define D_RUN " !Run:"
#define D_PIPE " !Pipe:"
#define _D_TOKENS " !Tokens:"


#ifdef DEBUG_OFF
#undef D_MAIN
#undef D_COMMANDS
#undef D_SIGNALS
#undef D_BUILTIN
#undef D_PARSER
#undef D_RUN
#undef D_PIPE
#undef D_TOKENS
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
void printContext(Context *cntx);
void printCommand(Command *cmd);
#endif

int parseline(Context *, char *);
int promptline(Context *, char *, int);
void contextNull(Context *);
int run(Context *, int);
void mySignalSet(void);

#endif // SHELL_H

