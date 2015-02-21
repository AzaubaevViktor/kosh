#ifndef SHELLERRORS_H
#define SHELLERRORS_H

enum ShellErrors {
    NoErr,
    InternalErr,
    TokenizerError,
    ParserError
};

enum ShellErrors _shellError;

char *getErrorStr(int err);

#define shellErrorRet(test, err) if (test) {_shellError = (err); return -1;}
#define shellError(test, err) if (test) {_shellError = (err); return 0;}
#define getShellError() (getErrorStr(_shellError))

#endif // SHELLERRORS_H

