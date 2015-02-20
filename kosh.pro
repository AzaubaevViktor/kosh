TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    shell.c \
    parseline.c \
    promptline.c \
    context.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    shell.h

