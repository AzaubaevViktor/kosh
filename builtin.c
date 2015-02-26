#include "shell.h"
#include "builtin.h"

extern char **environ;

int help(char *name, char **args, char **envs) {
    printf("Hello! This is KOrovin SHell.\n");
    printf("My pid:{%d}; my ppid:{%d}\n", getpid(), getppid());
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
    static char *res_s[1024] = {0};
    for (argc = 0; args[argc]; argc++) {}
    printf("СЕЙЧАС ЭТА ПРОГРАММА НЕ РАБОТАЕТ\n");
    if (3 == argc) {
        sprintf((char *) res_s, "%s=%s", args[1], args[2]);
        putenv((char *) res_s);
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
#ifdef D_BUILTIN
        printf("%s (\"%s\" == \"%s\")? ", D_BUILTIN, builtinCommands[i].name, name);
#endif
        if (strcmp(builtinCommands[i].name, name) == 0) {
#ifdef D_BUILTIN
            printf("YES\n");
#endif
            return builtinCommands[i].cmd;
        }
#ifdef D_BUILTIN
        else {
            printf("NO\n");
        }
#endif
    }
    return NULL;
}
