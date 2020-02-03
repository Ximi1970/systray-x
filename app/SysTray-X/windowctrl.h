#ifndef WINDOWCTRL_H
#define WINDOWCTRL_H

#include <QtGlobal>

#ifdef Q_OS_UNIX
#include "windowctrl-unix.h"
#endif // Q_OS_UNIX

#ifdef Q_OS_WIN
#include "windowctrl-win.h"
#endif // Q_OS_WIN

#endif // WINDOWCTRL_H
