#include "tokenizer.h"

extern enum ShellErrors _shellError;
extern char **environ;

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

char *makeDelim(int);
char *rightShift(char *);
char *findQuote(char *);
char *blankskip(register char *);
void argVariables(char *, char *);
char *argHandler(char *, char *);
#define curToken (tLine->tokens[ntok])
#define prevToken (tLine->tokens[ntok - 1])
#define confirmToken(_type, _data) {\
    curToken.type = (_type);\
    if (str == (_type)) {curToken.str = (_data);}\
    else {curToken.action = (enum Action) (_data);}\
    }
int tokenizer(TokensLine *tLine, char *line) {
    char *delim = makeDelim(false);
    char *delimQuote = makeDelim(true);
    char *s = line;
    char *_cmp = NULL;
    int ntok = 0;
    int i = 0;

    argVariables(s, delimQuote);
#ifdef D_TOKENS
    printf("%s \n", D_TOKENS);
#endif

    while (*s) {
#ifdef D_TOKENS
        printStrLine(line);
        printf("\n");
#endif
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
        if (isShellError()) {
            return -1;
        }

        ntok++;
    }

    return 0;
}

char *makeDelim(int withQuote) {
    int j = 0, i = 0;
    char *delim = calloc(ActionsCount + 10, sizeof(char));
    delim[i++] = ' ';
    delim[i++] = (withQuote) ? '\'' : ' ';
    delim[i++] = (withQuote) ? '\"' : ' ';
    delim[i++] = (withQuote) ? ':' : ' ';
    delim[i++] = '$';
    delim[i++] = '\t';
    delim[i++] = '\n';
    for (j = 0; j < ActionsCount; j++, i++) {
        delim[i] = actionSequences[j][0];
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

char *endStr(char *s) {
    while (*s) {
        s++;
    }
    return s;
}

void argVariables(char *s, char *delimQuote) {
    char *end = endStr(s);
    char *varEnd = NULL;
    char res = ' ';

    while (*s) {
        if ('$' == *s) {
            varEnd = strpbrk(s + 1, delimQuote);
            if (varEnd) {
                res = *varEnd;
                *varEnd = '\0';
            }
            char *envVal = getenv(s + 1);
            envVal = envVal ? envVal : "";

            unsigned int delta = strlen(envVal) - (strlen(s));
            if (varEnd)
                *varEnd = res;

            unsigned int i = 0;
            for (i = 0; i < delta; i++) {
                *(rightShift(s) - 1) = ' ';
                end += 1;
            }
            for (i = 0; i < -delta; i++) {
                leftShift(s);
                end -= 1;
            }

            for (i = 0; i < strlen(envVal); i++) {
                s[i] = envVal[i];
            }

            s += i;
        } else {
            s++;
        }
    }
}

char *findEnd(char *s, char *delim) {
    while (*s) {
        if (('\'' == *s) || ('\"' == *s)) {
            s = findQuote(s) + 1;
            continue;
        }
        if (s == strpbrk(s, delim)) {
            return s;
        }
        s++;
    }
    return s;
}

char *argHandler(char *s, char *delim) {
    char *end =  findEnd(s, delim);
    char *quote_end = NULL;
    end = rightShift(end) - 1;
    while (*s) {
        if (('\'' == *s) || ('\"' == *s)) {
            quote_end = findQuote(s) - 1;
            s = leftShift(s);
            shellError(!s, QuotesErr);
            s = leftShift(quote_end);
        }
        s++;
    }
    return end + 1;
}
