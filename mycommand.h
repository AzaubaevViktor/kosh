#ifndef MYCOMMAND_H
#define MYCOMMAND_H
#include <stdlib.h>
#include <string.h>

typedef int *(BuiltinCmdType)(char *, char **, char **);

typedef struct _BuiltinCommand {
    char *name;
    BuiltinCmdType *cmd;
} BuiltinCommand;

BuiltinCommand builtinCommands[];
BuiltinCmdType *getCmdByName(char *name);

#endif // MYCOMMAND_H

