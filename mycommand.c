#include "mycommand.h"

extern char **environ;

int help(char *name, char **args, char **envs) {
//int help(char *name, char **args, char **envs) {
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

int set(char *name, char **args, char **environ) {
    int argc = 0;
    for (argc = 0; args[argc]; argc++) {}
    printf("СЕЙЧАС ЭТА ПРОГРАММА НЕ РАБОТАЕТ\n");
    if (3 == argc) {
        static char *res_s[1024] = {0};
        sprintf(res_s, "%s=%s", args[1], args[2]);
        putenv(res_s);
        return 0;
    } else if (3 < argc) {
        printf("To more arguments\n");
        return 1;
    } else if (3 > argc) {
        printf("To few arguments\n");
        return 1;
    }
    return 0;
}

BuiltinCommand
builtinCommands[] = {
    {"help", help},
    {"set", set},
    {NULL, NULL}
};

BuiltinCmdType *getCmdByName(char *name) {
    int i = 0;
    for (i = 0; builtinCommands[i].name; i++) {
//        printf("# %s -- %s\n", builtinCommands[i].name, name);
        if (strcmp(builtinCommands[i].name, name) == 0) {
            return builtinCommands[i].cmd;
        }
    }
    return NULL;
}
