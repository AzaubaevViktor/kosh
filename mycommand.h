#ifndef MYCOMMAND_H
#define MYCOMMAND_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

typedef int (*BuiltinCmdType)(char *, char **, char **);

typedef struct _BuiltinCommand {
    char *name;
    BuiltinCmdType *cmd;
} BuiltinCommand;

BuiltinCmdType *getCmdByName(char *name);

#endif // MYCOMMAND_H

