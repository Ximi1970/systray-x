#include "windowctrl-win.h"

#ifdef Q_OS_WIN

/*
 *  Constructor
 */
WindowCtrlWin::WindowCtrlWin( QObject *parent) : QObject( parent )
{

}


/*
 *  Find the window with title
 */
bool    WindowCtrlWin::findWindow( const QString& title )
{
    return false;
}


/*
 *  Display the window elements
 */
void    WindowCtrlWin::displayWindowElements( const QString& title )
{
}


/*
 *  Get the Thunderbird window ID
 */
QList< WinId >   WindowCtrlWin::getWinIds()
{
    return m_tb_windows;
}


/*
 *  Minimize a window
 */
void    WindowCtrlWin::minimizeWindow( WinId window )
{
}


/*
 *  Normalize a window
 */
void    WindowCtrlWin::normalizeWindow( WinId window )
{
}


/*
 *  Hide a window
 */
void    WindowCtrlWin::hideWindow( WinId window, bool state )
{
}

#endif // Q_OS_WIN
