#ifndef DEBUGUTIL
#define DEBUGUTIL

#define DEBUG_STR "\e[31m[%s]\e[m "

#define debugSimple(type, msg) \
if (1 == type) {\
    printf(DEBUG_STR msg "\n", #type); \
}

#define debug(type, msg, ...) \
if (1 == type) {\
    printf(DEBUG_STR msg "\n", #type, __VA_ARGS__); \
}


#endif // DEBUGUTIL

