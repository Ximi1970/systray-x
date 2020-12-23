#
#	Remove core Qt libs
#
QT       -= core gui

TARGET = SysTray-X-x11
TEMPLATE = lib

CONFIG += staticlib

include(../SysTray-X.pri)

DEFINES += SysTray_X_X11_LIBRARY

#
#	Clear Qt dependencies
#
QMAKE_INCDIR =
QMAKE_INCDIR_QT =
QMAKE_LIBDIR_QT =


SOURCES += systray-x-lib.cpp

HEADERS += systray-x-lib.h
