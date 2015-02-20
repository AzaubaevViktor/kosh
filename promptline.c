#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "shell.h"

int promptline(char *prompt, char *line, int sizline)
{
    int n = 0;

    write(1, prompt, strlen(prompt));
    while (1) {
        n += read(0, (line + n), sizline-n);
        *(line+n) = '\0';

        if (*(line+n-2) == '\\' && *(line+n-1) == '\n') {
//            *(line+n) = ' ';
            *(line+n-1) = '\0';
            *(line+n-2) = '\0';
            n -= 2;
            write(1, "$>", 3);
            continue;   /*  read next line  */
        }

        return(n);      /* all done */
    }
}
