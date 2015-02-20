#include "shell.h"
#include "tokenizer.h"

char *actionSequences[ActionsCount] =
            {"|",      "&",       "<",    ">>",    ">",     ";"};

void clearTokensLine(TokensLine *tLine) {
    int i = 0;
    for (i = 0; i < MAXTOKENS; i++) {
        tLine->tokens[i].type = empty;
        tLine->tokens[i].str = (char *) NULL;
    }
}

#define curToken (tLine->tokens[i])
void printTokensLine(TokensLine *tLine) {
    int i = 0;
    static char *typesStr[] = {"∅", "S", "Q", "A"};

    printf("TOKENS:\n");

    while (empty != curToken.type) {
        printf("%s", typesStr[curToken.type]);
        if (action == curToken.type)
            printf("[%d]", curToken.action);
        else
            printf("[%s]", curToken.str);
        printf(" ");
        i++;
    }
    printf("\n");
}

char *makeDelim();
char *rightShift(char *);
char *findQuote(char *);
char *blankskip(register char *);
#define curToken (tLine->tokens[ntok])
int tokenizer(TokensLine *tLine, char *line) {
    char *delim = makeDelim();
    char *s = line;
    char *_cmp = NULL;
    int ntok = 0;
    int i = 0;

    while (*s) {
        /* Search first comparing */
        s = blankskip(s);
        if (!*s)
            break;

        if (('\'' == *s) || ('"' == *s)) {
            curToken.type = quotedStr;
            curToken.str = s;
            s = findQuote(s);

            if (!s) {
                fprintf(stderr, "syntax error\n");
                return -1;
            }

            s = rightShift(++s);
            ntok++;
            continue;
        }

        for(i = 0; i < ActionsCount; i++) {
            _cmp = strstr(s, actionSequences[i]);
            if (_cmp == s) {
                curToken.type = action;
                curToken.action = (enum Action) i;
                s += strlen(actionSequences[i]) - 1;
                *s++ = '\0';
                break;
            }
        }

        if (empty != curToken.type) {
            ntok++;
            continue;
        }

        curToken.type = str;
        curToken.str = s;

        s = strpbrk(s, delim);
        s = rightShift(s);

        ntok++;
    }
    return 0;
}

char *makeDelim() {
    int i = 0;
    char *delim = calloc(ActionsCount + 10, sizeof(char));
    delim[0] = ' ';
    delim[1] = '\t';
    delim[2] = '\n';
    for (i = 0; i < ActionsCount; i++) {
        delim[3 + i] = actionSequences[i][0];
    }
    return delim;
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

char *findQuote(char *str) {
    /* Ищет *str в str. Если не находит, возвращает NULL */
    char *s = str;
    char quote = *str;

    while (*s) {
        if ((str != s) && (quote == *s) && ('\\' != *(s-1))) {
            return s;
        }
        s++;
    }
    return NULL;
}

char *blankskip(register char *s)
{
    while (isspace(*s) && *s) ++s;
    return(s);
}
