#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "shell.h"

void promptMake(Context *cntx, char* prompt) {
    time_t current = time(NULL);
    sprintf(prompt, "[%s] {%d} %s>", cntx->argv[0], getpid(), ctime(&current));
}

#define add_char { *(line + n) = (char) ch; n++; }

int promptline(Context *cntx, char *line, int fromFile) {
    int n = 0;
    static char prompt[1024];
    int ch;

    if (!fromFile) {
        promptMake(cntx, prompt);
        write(1, prompt, strlen(prompt));
    }

    while (1) {
        while (1) {
            ch = getchar();
            if ((ch == '\n') || (-1 == ch)) {
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

        return n;      /* all done */
    }
}
