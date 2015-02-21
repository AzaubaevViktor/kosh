#include "mycommand.h"

int help(char *name, char **args, char **envs) {
    printf("Hello! This is KOrovin SHell.\n");
    printf("ARGS:\n");
    char **curargs = args;
    while (*curargs) {
        printf(" > %s\n", *curargs);
        curargs++;
    }
    printf("ENVIRONMENTS:\n");
    char **curenv = envs;
    while (*curenv) {
        printf(" > %s\n", *curenv);
        curenv++;
    }
    return 0;
}

BuiltinCommand
builtinCommands[] = {
    {"help", help}
};

builtinCmd *getCmdByName(char *name) {
    int i = 0;
    for (i = 0; builtinCommands[i].name; i++) {
        printf("# %s -- %s\n", builtinCommands[i].name, name);
        if (strcmp(builtinCommands[i].name, name) == 0) {
            return builtinCommands[i].cmd;
        }
    }
    return NULL;
}
