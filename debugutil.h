#ifndef DEBUGUTIL
#define DEBUGUTIL

#define DEBUG_STR "\e[31m[%s]\e[m "

#define debugSimple(type, msg) \
if (1 == type) {\
    printf(DEBUG_STR "`" __FILE__ ":%d`" msg "\n", #type, __LINE__); \
}

#define debug(type, msg, ...) \
if (1 == type) {\
    printf(DEBUG_STR "`" __FILE__ ":%d` " msg "\n",\
    #type, __LINE__, __VA_ARGS__); \
}


#endif // DEBUGUTIL

