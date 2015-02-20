#ifndef SHELL_H
#define SHELL_H

#define MAXARGS (256)
#define MAXCMDS (50)
#define LINELEN (2048)

/*  cmdflag's  */
#define OUTPIP  (01)
#define INPIP   (02)
#define BACKGROUND (04)

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

int parseline(Context *cntx, char *);
int promptline(Context *, char *, int);
void contextNull(Context *cntx);
void printContext(Context *cntx);

extern int shellError;

#endif // SHELL_H

