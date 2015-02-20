#ifndef SHELL_H
#define SHELL_H

#define MAXARGS 256
#define MAXCMDS 50

typedef struct _Command {
    char *cmdargs[MAXARGS];
    char cmdflag;
} Command;

/*  cmdflag's  */
#define OUTPIP  01
#define INPIP   02

typedef struct _Context {
    Command cmds[MAXCMDS];
    char *infile, *outfile, *appfile;
    char bkgrnd;
} Context;

int parseline(Context *cntx, char *);
int promptline(char *, char *, int);
void contextNull(Context *cntx);

#endif // SHELL_H

