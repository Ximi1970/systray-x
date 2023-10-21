#
#	Remove core Qt libs
#
QT       -= core gui

TARGET = SysTray-X-wayland
TEMPLATE = lib

CONFIG += staticlib

include(../SysTray-X.pri)

#
#	Clear Qt dependencies
#
QMAKE_INCDIR =
QMAKE_INCDIR_QT =
QMAKE_LIBDIR_QT =


SOURCES += systray-x-lib-wayland.cpp

HEADERS += systray-x-lib-wayland.h
