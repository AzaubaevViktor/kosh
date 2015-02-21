#ifndef TOKENIZER
#define TOKENIZER
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "shell.h"

#define MAXTOKENS (LINELEN/2)

enum TokenType {empty, str, action};
enum Action {conveyor, background, infile, appfile, outfile, semicolon, _end};
#define ActionsCount ((int) _end)

typedef struct _Token {
    enum TokenType type;
    union {
        char *str;
        enum Action action;
    };
} Token;

typedef struct _TokensLine {
    Token tokens[MAXTOKENS];
} TokensLine;

void clearTokensLine(TokensLine *tLine);
void printTokensLine(TokensLine *tLine);
int tokenizer(TokensLine *tLine, char *line);


#endif // TOKENIZER

