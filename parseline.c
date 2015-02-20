
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "shell.h"
char *rightShift(char *str);
char *findQuote(char *str, char quote);
static char *blankskip(register char *);

int parseline(Context *cntx, char *line)
{
    int nargs, ncmds;
    register char *s;
    char aflg = 0;
    int rval;
    register int i;
    static char delim[] = " \t|&<>;\n";
    char quote = '\0';

    /* initialize  */
    nargs = ncmds = rval = 0;
    s = line;

    contextNull(cntx);

    while (*s) {        /* until line has been parsed */
        s = blankskip(s);       /*  skip white space */
        if (!*s) break; /*  done with line */

        /*  handle <, >, |, &, and ;  */
        switch(*s) {
        case '&':
            ++(cntx->bkgrnd);
            *s++ = '\0';
            break;
        case '>':
            if (*(s+1) == '>') {
                ++aflg;
                *s++ = '\0';
            }
            *s++ = '\0';
            s = blankskip(s);
            if (!*s) {
                fprintf(stderr, "syntax error\n");
                return(-1);
            }

            if (aflg)
                cntx->appfile = s;
            else
                cntx->outfile = s;
            s = strpbrk(s, delim);
            if (isspace(*s))
                *s++ = '\0';
            break;
        case '<':
            *s++ = '\0';
            s = blankskip(s);
            if (!*s) {
                fprintf(stderr, "syntax error\n");
                return(-1);
            }
            cntx->infile = s;
            s = strpbrk(s, delim);
            if (isspace(*s))
                *s++ = '\0';
            break;
        case '|':
            if (nargs == 0) {
                fprintf(stderr, "syntax error\n");
                return(-1);
            }
            cntx->cmds[ncmds++].cmdflag |= OUTPIP;
            cntx->cmds[ncmds].cmdflag |= INPIP;
            *s++ = '\0';
            nargs = 0;
            break;
        case ';':
            *s++ = '\0';
            ++ncmds;
            nargs = 0;
            break;
        case '\"':
            quote = '\"';
        case '\'':
            quote = !quote ? '\'' : quote;

            cntx->cmds[ncmds].cmdargs[nargs++] = s;
            cntx->cmds[ncmds].cmdargs[nargs] = (char *) NULL;

            s = findQuote(s, quote);

            if (!s) {
                fprintf(stderr, "syntax error\n");
                return -1;
            }

            s = rightShift(++s);

            quote = 0;
            break;
        default:
            /*  a command argument  */
            if (nargs == 0) /* next command */
                rval = ncmds+1;
            cntx->cmds[ncmds].cmdargs[nargs++] = s;
            cntx->cmds[ncmds].cmdargs[nargs] = (char *) NULL;
            s = strpbrk(s, delim);
            if (isspace(*s))
                *s++ = '\0';
            break;
        }  /*  close switch  */
    }  /* close while  */

    /*  error check  */

    /*
          *  The only errors that will be checked for are
      *  no command on the right side of a pipe
          *  no command to the left of a pipe is checked above
      */
    if (cntx->cmds[ncmds-1].cmdflag & OUTPIP) {
        if (nargs == 0) {
            fprintf(stderr, "syntax error\n");
            return(-1);
        }
    }

    cntx->ncmds = rval;

    return(rval);
}

char *rightShift(char *str) {
    char *s = str;
    while (*s) {
        s++;
    }
    *(s + 1) = '\0';
    while (s != str) {
        *s = *(s - 1);
        s--;
    }

    *s = '\0';
    return s + 1;
}

char *findQuote(char *str, char quote) {
    /* Ищет quote в str. Если не находит, возвращает NULL */
    char *s = str;
    while (*s) {
        if ((str != s) && (quote == *s) && ('\\' != *(s-1))) {
            return s;
        }
        s++;
    }
    return NULL;
}

static char *blankskip(register char *s)
{
    while (isspace(*s) && *s) ++s;
    return(s);
}
