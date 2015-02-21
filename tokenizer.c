#include "tokenizer.h"

extern enum ShellErrors _shellError;

char *actionSequences[ActionsCount] =
{"|",      "&",       "<",    ">>",    ">",     ";"};
char quotes[] = "\'\"";

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
char *argHandler(char *s, char *delim);
#define curToken (tLine->tokens[ntok])
#define prevToken (tLine->tokens[ntok - 1])
#define confirmToken(_type, _data) {curToken.type = (_type); if (str == (_type)) {curToken.str = (_data);} else {curToken.action = (enum Action) (_data);}}
int tokenizer(TokensLine *tLine, char *line) {
    char *delim = makeDelim();
    char *s = line;
    char *_cmp = NULL;
    int ntok = 0;
    int i = 0;

    while (*s) {
        for (i = 0; '\n' != line[i]; i++) {
            printf("%3.d[%1c] ", line[i], line[i]);
        }
        printf("\n");
        /* Search first comparing */
        s = blankskip(s);

        if (!*s)
            break;

        // Action
        for(i = 0; i < ActionsCount; i++) {
            _cmp = strstr(s, actionSequences[i]);
            if (_cmp == s) {
                confirmToken(action, i);
                s += strlen(actionSequences[i]) - 1;
                *s++ = '\0';
                break;
            }
        }

        // String
        if (empty != curToken.type) {
            ntok++;
            continue;
        }
        confirmToken(str, s);

        s = argHandler(s, delim);

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

char *leftShift(char *str) {
    char *s = str;

    while (*s) {
        *s = *(s + 1);
        s++;
    }

    *(s - 1) = '\0';
    return str;
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

char *argHandler(char *s, char *delim) {
    char *end = strpbrk(s, delim);
    char *quote_end = NULL;
    end = rightShift(end) - 1;
    while (*s) {
        if (('\'' == *s) || ('\"' == *s)) {
            quote_end = findQuote(s) - 1;
            s = leftShift(s);
            shellError(!s, TokenizerError);
            s = leftShift(quote_end);
        }
        s++;
    }
    return end + 1;
}
