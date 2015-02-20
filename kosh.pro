TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    shell.c \
    parseline.c \
    promtline.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    shell.h

