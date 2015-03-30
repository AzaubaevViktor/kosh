#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "shell.h"

void promptMake(Context *cntx, char* prompt) {
    time_t current = time(NULL);
    sprintf(prompt, "[%s] {%d} %s>", cntx->argv[0], getpid(), ctime(&current));
}

void printPrompt(Context *cntx) {
    static char prompt[1024];
    if (!cntx->fromFile) {
        promptMake(cntx, prompt);
        write(1, prompt, strlen(prompt));
    }
}

#define add_char { *(line + n) = (char) ch; n++; }

void readCmds(Context *cntx, char *line) {
    int n = 0;
    int ch;

   // printPrompt(cntx);

    while (1) {
        while (1) {
            if (cntx->fromFile) {
                ch = getc(cntx->gInp);
                if (-1 == ch) {
                    exit(EXIT_SUCCESS);
                }
            } else {
                while (-1 == (ch = getchar()));
            }
            if (ch == '\n') {
                add_char;
                break;
            }
            add_char;
        }
        *(line+n) = '\0';

        if (*(line+n-2) == '\\' && *(line+n-1) == '\n') {
            *(line+n-1) = '\0';
            *(line+n-2) = '\0';
            n -= 2;
            write(1, "$>", 3);
            continue;   /*  read next line  */
        }

        return;      /* all done */
    }
}
