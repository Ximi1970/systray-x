#include "windowctrl-win.h"

#ifdef Q_OS_WIN

/*
 *  System includes
 */


/*
 *  Statics
 */
QList< quint64 >  WindowCtrlWin::m_tb_windows;


/*
 *  Constructor
 */
WindowCtrlWin::WindowCtrlWin( QObject *parent) : QObject( parent )
{
    m_tb_windows = QList< quint64 >();
}


/*
 *  Find the window with title
 */
bool    WindowCtrlWin::findWindow( const QString& title )
{
    m_tb_windows = QList< quint64 >();

    EnumWindows( &EnumWindowsProc, (LPARAM)(LPSTR)( title.toStdString().c_str() ) );

    return false;
}


/*
 *  Display the window elements
 */
void    WindowCtrlWin::displayWindowElements( const QString& title )
{
    findWindow( title );

    foreach( quint64 win_id, getWinIds() )
    {
        emit signalConsole( QString( "Found: XID %1" ).arg( win_id ) );
    }
}


/*
 *  Callback for the window enumaration
 */
BOOL CALLBACK   WindowCtrlWin::EnumWindowsProc( HWND hwnd, LPARAM lParam )
{
    char buffer[ 128 ];
    int written = GetWindowTextA( hwnd, buffer, 128 );
    if( written && strstr( buffer, (char*)lParam ) != NULL )
    {
        m_tb_windows.append( (quint64)hwnd );
    }

    return TRUE;
}


/*
 *  Get the Thunderbird window ID
 */
QList< quint64 >   WindowCtrlWin::getWinIds()
{
    return m_tb_windows;
}


/*
 *  Minimize a window
 */
void    WindowCtrlWin::minimizeWindow( quint64 window )
{
}


/*
 *  Normalize a window
 */
void    WindowCtrlWin::normalizeWindow( quint64 window )
{
}


/*
 *  Hide a window
 */
void    WindowCtrlWin::hideWindow( quint64 window, bool state )
{
}

#endif // Q_OS_WIN
