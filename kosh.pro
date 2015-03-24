TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    shell.c \
    parseline.c \
    context.c \
    debug.c \
    promptline.c \
    tokenizer.c \
    shellerrors.c \
    run.c \
    mysignals.c \
    builtin.c \
    jobs.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    shell.h \
    tokenizer.h \
    shellerrors.h \
    builtin.h \
    debugutil.h

DISTFILES += \
    TODO.md \
    test.ksh

