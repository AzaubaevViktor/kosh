#include "shellerrors.h"
char *getErrorStr(int err) {
    static char *errMsgs[] = {
        "No error",
        "Internal Error"
    };
    return errMsgs[err];
}
