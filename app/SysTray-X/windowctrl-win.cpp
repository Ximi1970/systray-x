#include "windowctrl-win.h"

#ifdef Q_OS_WIN

/*
 *  System includes
 */


/*
 *  Statics
 */
QList< quint64 >  WindowCtrlWin::m_tb_windows;
QList< QRect >  WindowCtrlWin::m_tb_geometries;


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

    if( m_tb_windows.length() == 0 )
    {
        return false;
    }

    return true;
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

        RECT rect;
        if( GetWindowRect(hwnd, &rect) )
        {
            m_tb_geometries.append( QRect( QPoint( rect.left, rect.top ), QPoint( rect.right, rect.bottom ) ) );
        }
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
 *  Get the Thunderbird window geometries
 */
QList< QRect >   WindowCtrlWin::getWinGeos()
{
    return m_tb_geometries;
}


/*
 *  Minimize a window
 */
void    WindowCtrlWin::minimizeWindow( quint64 window, bool hide )
{
    ShowWindow( (HWND)window, SW_MINIMIZE );

    if( hide )
    {
        hideWindow( (HWND)window );
    }
}


/*
 *  Normalize a window
 */
void    WindowCtrlWin::normalizeWindow( quint64 window )
{
    ShowWindow( (HWND)window, SW_RESTORE );
    SetForegroundWindow( (HWND)window );
}


/*
 *  Hide a window
 */
void    WindowCtrlWin::hideWindow( HWND hwnd )
{
    emit signalConsole( "Hide" );

    long style = GetWindowLong( hwnd, GWL_STYLE );

    style &= ~(WS_VISIBLE);
    style |= WS_EX_TOOLWINDOW;
    style &= ~(WS_EX_APPWINDOW);

    SetWindowLong( hwnd, GWL_STYLE, style );
}


/*
 *  Remove minimize, maximize, close, moving a window
 */
void    WindowCtrlWin::changeStyleWindow( HWND hwnd )
{
    emit signalConsole( QString("Set style for %1").arg((quint64)hwnd) );

    long style = GetWindowLong( hwnd, GWL_STYLE );

    emit signalConsole( QString("Has style %1").arg((quint64)style) );

//    WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_BORDER | WS_DLGFRAME |
//    WS_SYSMENU | WS_SIZEBOX | WS_GROUP | WS_TABSTOP


//    style &= ~(WS_VISIBLE);
//    style |= WS_EX_TOOLWINDOW;
//    style &= ~(WS_EX_APPWINDOW);

//    long style = (WS_BORDER | WS_CAPTION) & (~WS_ICONIC);
//    SetWindowLong( hwnd, GWL_STYLE, GetWindowLong( hwnd, GWL_STYLE)&~WS_SIZEBOX);

    style &= ~(WS_BORDER);
    style &= ~(WS_DLGFRAME);
    style &= ~(WS_SIZEBOX);

    SetWindowLongPtr( hwnd, GWL_STYLE, style );
}

#endif // Q_OS_WIN
