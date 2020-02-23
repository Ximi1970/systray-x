#include "windowctrl-win.h"

#ifdef Q_OS_WIN

/*
 *  System includes
 */
#include <CommCtrl.h>

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
    long style = GetWindowLong( hwnd, GWL_STYLE );

    style &= ~(WS_VISIBLE);
    style |= WS_EX_TOOLWINDOW;
    style &= ~(WS_EX_APPWINDOW);

    SetWindowLong( hwnd, GWL_STYLE, style );
}





/*
LRESULT CALLBACK    WindowCtrlWin::mySubClassProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData )
{
    MessageBoxA( NULL, "Test", "Test", MB_OK );

    switch(uMsg)
    {
        case WM_LBUTTONDOWN:
            MessageBoxA( NULL, "Button down!", "Debug", MB_OK );
            break;

        case WM_NCDESTROY:
            RemoveWindowSubclass( hWnd, &mySubClassProc, 1 );
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
*/

/*
 *  Callback for the window enumaration
 */
LRESULT CALLBACK   WindowCtrlWin::WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    MessageBoxA( NULL, "Test", "Test", MB_OK );

    return TRUE;

    if( uMsg == WM_CLOSE )
    {
        return TRUE;
    }

//    return CallWindowProc( prev, hwnd, uMsg,wParam, lParam);

    return DefWindowProc( hwnd, uMsg, wParam, lParam );
}


/*
 *  Close experiment
 */
void    WindowCtrlWin::closeWindow( HWND hwnd )
{
    emit signalConsole("Close Window intercept");

//    SetWindowSubclass( hwnd, &mySubClassProc, 1, 0);

    MessageBoxA( NULL, "Start test", "Test", MB_OK );

    WNDPROC prev = (WNDPROC)SetWindowLongPtr( hwnd, GWLP_WNDPROC, (LONG_PTR)&WindowCtrlWin::WindowProc );
}

#endif // Q_OS_WIN
