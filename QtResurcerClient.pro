QT += core network gui widgets

TARGET = QtResurcerClient
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS


CONFIG += debug_and_release

SOURCES += \
    client_main.cpp \
    client_window.cpp \
    client.cpp

FORMS += \
    client_window.ui

HEADERS += \
    client_window.h \
    client.h
