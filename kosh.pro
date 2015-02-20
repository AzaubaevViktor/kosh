TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    shell.c \
    parseline.c \
    context.c \
    debug.c \
    promptline.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    shell.h

