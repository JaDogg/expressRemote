#-------------------------------------------------
#
# Project created by QtCreator 2014-12-20T09:32:39
#
#-------------------------------------------------

QT       += core widgets

TARGET = remote
CONFIG   += console
CONFIG   -= app_bundle

QMAKE_CXXFLAGS += -std=c++11

TEMPLATE = app

INCLUDEPATH += botan/ ssh/
DEPENDPATH += botan/ ssh/

include(botan/botan.pri)
include(ssh/ssh.pri)

SOURCES += main.cpp \
    expressremote.cpp \
    argparser.cpp
HEADERS += \
    expressremote.h \
    argparser.h

RC_FILE = WindowsResources/win_rsrc.rc
