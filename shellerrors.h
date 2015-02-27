#ifndef SHELLERRORS_H
#define SHELLERRORS_H

enum ShellErrors {
    NoErr,
    InternalErr,
    ParserErr,
    QuotesErr,
    TokenizerErr,
    ParserError,
    FileOpenError
};

enum ShellErrors _shellError;

char *getErrorStr(int err);

#define shellErrorRet(test, err) if (test) {\
    _shellError = (err);\
    printf("%s %d", __FILE__, __LINE__);\
    return -1;\
    }
#define shellError(test, err) if (test) {\
    _shellError = (err);\
    printf("%s %d", __FILE__, __LINE__);\
    return 0;\
    }
#define getShellError() (getErrorStr(_shellError))
#define isShellError() (NoErr != _shellError)

#endif // SHELLERRORS_H

