#ifndef SHELLERRORS_H
#define SHELLERRORS_H

enum ShellErrors {
    NoErr,
    InternalErr,
    TokenizerError,
    ParserError
};

enum ShellErrors _shellError;

char *getError(int err);

#define shellErrorRet(test, err) if (test) {_shellError = (err); return -1;}
#define shellError(test, err) if (test) {_shellError = (err); return 0;}
#define getShellError() (getError(_shellError))

#endif // SHELLERRORS_H

