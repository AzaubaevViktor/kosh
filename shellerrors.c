#include "shellerrors.h"
char *getError(int err) {
    static char *errMsgs[] = {
        "No error",
        "Internal Error"
    };
    return errMsgs[err];
}
