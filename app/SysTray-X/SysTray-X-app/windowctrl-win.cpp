#include "windowctrl-win.h"

#ifdef Q_OS_WIN

/*
 *  System includes
 */
#include <TlHelp32.h>
#include <Psapi.h>
#include <CommCtrl.h>

/*
 * Standard library includes
 */
#include <array>

/*
 * Qt includes
 */
#include <QCoreApplication>
#include <QString>


/*
 *  Initialize statics
 */
WindowCtrlWin*  WindowCtrlWin::m_ctrl_parent = nullptr;


/*
 *  Constructor
 */
WindowCtrlWin::WindowCtrlWin( QObject *parent) : QObject( parent )
{
    /*
     *  Initialize
     */
    m_tb_windows = QList< quint64 >();
    m_tb_windows_hidden = QList< quint64 >();
    m_tb_window_states = QMap< quint64, Preferences::WindowState >();

    /*
     * Setup the minimize intercept
     */
    m_ctrl_parent = this;
    interceptMinimizeWindow();
}


/*
 * Destructor
 */
WindowCtrlWin::~WindowCtrlWin()
{
    /*
     * Remove the intercept hook
     */
    stopInterceptMinimizeWindow();
}


/*
 *  Get the states of the TB windows.
 */
const Preferences::WindowState&    WindowCtrlWin::getWindowState( const quint64 window )
{
    return m_tb_window_states[ window ];
}


/*
 *  Set the minimize type
 */
void    WindowCtrlWin::setMinimizeType( Preferences::MinimizeType type )
{
    m_minimize_type = type;
}


/*
 *  Get the minimize type
 */
Preferences::MinimizeType    WindowCtrlWin::getMinimizeType() const
{
    return m_minimize_type;
}


/*
 *  Set the minimize icon type
 */
void    WindowCtrlWin::setMinimizeIconType( Preferences::MinimizeIconType type )
{
    m_minimize_icon_type = type;
}


/*
 *  Get the minimize icon type
 */
Preferences::MinimizeIconType    WindowCtrlWin::getMinimizeIconType() const
{
    return m_minimize_icon_type;
}


/*
 *  Set the minimize type
 */
void    WindowCtrlWin::setCloseType( Preferences::CloseType type )
{
    m_close_type = type;
}


/*
 *  Get the close type
 */
Preferences::CloseType    WindowCtrlWin::getCloseType() const
{
    return m_close_type;
}


/*
 *  Get the parent pid of SysTray-X, TB hopefully
 */
qint64  WindowCtrlWin::getPpid() const
{
    HANDLE h = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof( PROCESSENTRY32 );

    qint64 ppid = -1;
    qint64 pid = QCoreApplication::applicationPid();

    if( Process32First( h, &pe ) )
    {
        do
        {
            if( pe.th32ProcessID == pid )
            {
               ppid = pe.th32ParentProcessID;
               break;
            }
        }
        while( Process32Next( h, &pe ) );
    }

    CloseHandle( h );

    return ppid;
}


/*
 *  Is the pid from thunderbird
 */
bool    WindowCtrlWin::isThunderbird( qint64 pid ) const
{
    return getProcessName( pid ).contains( "thunderbird", Qt::CaseInsensitive );
}


/*
 *  Get the process name
 */
QString WindowCtrlWin::getProcessName( qint64 pid ) const
{
    HANDLE proc = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pid );
    char name[ 256 ];
    GetModuleBaseNameA( proc, nullptr, name, 256);

    return QString( name );
}


/*
 *  Find the window by title
 */
bool    WindowCtrlWin::findWindow( const QString& title )
{
    m_tb_windows = QList< quint64 >();

    EnumWindowsTitleProcData data{ *this, title.toStdString() };

    EnumWindows( &enumWindowsTitleProc, reinterpret_cast<LPARAM>(&data) );

    if( m_tb_windows.length() == 0 )
    {
        return false;
    }

    return true;
}


/*
 *  Callback for the window enumaration title find.
 */
BOOL CALLBACK   WindowCtrlWin::enumWindowsTitleProc( HWND hwnd, LPARAM lParam )
{
    auto& data = *reinterpret_cast<EnumWindowsTitleProcData*>(lParam);
    std::array<char, 128> buffer;
    int written = GetWindowTextA( hwnd, buffer.data(), int(buffer.size()) );
    if( written && strstr( buffer.data(), data.title.c_str() ) != nullptr )
    {
        data.window_ctrl.m_tb_windows.append( (quint64)hwnd );
    }

    return TRUE;
}


/*
 *  Find the window by pid
 */
void    WindowCtrlWin::findWindows( qint64 pid )
{
    m_tb_windows = QList< quint64 >();
    m_tb_window_states = QMap< quint64, Preferences::WindowState >();

    EnumWindowsPidProcData data{ *this, pid };
    EnumWindows( &enumWindowsPidProc, reinterpret_cast<LPARAM>(&data) );

 /*
    emit signalConsole( QString( "Number of windows found: %1" ).arg( m_tb_windows.length() ) );

    for( int i = 0 ; i< m_tb_windows.length() ; ++i )
    {
        emit signalConsole( QString( "WinID %1, State %2" ).arg( m_tb_windows.at( i ) ).arg( m_tb_window_states.at( i ) ) );
    }
 */
}


/*
 *  Callback for the window enumaration pid find.
 */
BOOL CALLBACK   WindowCtrlWin::enumWindowsPidProc( HWND hWnd, LPARAM lParam )
{
    auto& data = *reinterpret_cast<EnumWindowsPidProcData*>(lParam);
    unsigned long pid = 0;

    GetWindowThreadProcessId( hWnd, &pid );

    if( data.pid != pid || !isMainWindow( hWnd ) )
    {
        return TRUE;
    }

    /*
     *  Store the window id
     */
    data.window_ctrl.m_tb_windows.append( (quint64)hWnd );

    /*
     *  Get the window state
     */
    WINDOWPLACEMENT wp;
    wp.length = sizeof( WINDOWPLACEMENT );
    GetWindowPlacement( hWnd, &wp );

    if( SW_SHOWMINIMIZED == wp.showCmd )
    {
        /*
         * Window is minimized
         */
        data.window_ctrl.m_tb_window_states[ (quint64)hWnd ] = Preferences::STATE_MINIMIZED;
    }
    else
    {
        data.window_ctrl.m_tb_window_states[ (quint64)hWnd ] = Preferences::STATE_NORMAL;
    }

    return TRUE;
}


/*
 *  Check for main window
 */
BOOL    WindowCtrlWin::isMainWindow( HWND hwnd )
{
    return GetWindow( hwnd, GW_OWNER ) == nullptr && ( IsWindowVisible( hwnd ) || IsIconic( hwnd ) );
}


/*
 *  Display the window elements
 */
void    WindowCtrlWin::displayWindowElements( const QString& title )
{
    findWindow( title );

    QList< quint64 > winIds = getWinIds();
    for( int i = 0 ; i < winIds.length() ; i++ )
    {
        emit signalConsole( QString( "Found: XID %1" ).arg( winIds[ i ] ) );
    }
}


/*
 *  Display the window elements
 */
void    WindowCtrlWin::displayWindowElements( quint64 window )
{
    emit signalConsole( QString( "Found: XID %1" ).arg( window ) );
}


/*
 *  Get the Thunderbird window IDs
 */
QList< quint64 >   WindowCtrlWin::getWinIds()
{
    return m_tb_windows;
}


/*
 * Intercept the minimize event
 */
void    WindowCtrlWin::interceptMinimizeWindow()
{
    m_hook = SetWinEventHook(
                (DWORD)EVENT_OBJECT_LOCATIONCHANGE, (DWORD)EVENT_OBJECT_LOCATIONCHANGE,
                (HMODULE)NULL,
                (WINEVENTPROC)handleWinEvent,
                (DWORD)getPpid(), (DWORD)0,
                (DWORD)WINEVENT_OUTOFCONTEXT );
}


/*
 * Remove the intercept hook
 */
void    WindowCtrlWin::stopInterceptMinimizeWindow()
{
    UnhookWinEvent( m_hook );
}


/*
 * Hook callback
 */
void CALLBACK   WindowCtrlWin::handleWinEvent( HWINEVENTHOOK hook, DWORD event, HWND hWnd,
                         LONG idObject, LONG idChild,
                         DWORD dwEventThread, DWORD dwmsEventTime )
{
    Q_UNUSED( hook );
    Q_UNUSED( idObject );
    Q_UNUSED( idChild );
    Q_UNUSED( dwEventThread );
    Q_UNUSED( dwmsEventTime );

    if( EVENT_OBJECT_LOCATIONCHANGE == event )
    {
        WINDOWPLACEMENT wp;
        wp.length = sizeof( WINDOWPLACEMENT );
        GetWindowPlacement( hWnd, &wp );

        if( SW_SHOWMINIMIZED == wp.showCmd )
        {
            /*
             * Window is minimized
             */
            if( m_ctrl_parent )
            {
                m_ctrl_parent->hookAction( hWnd );
            }
        }
    }
}


/*
 * Non-static method to use by the hook callback
 */
void    WindowCtrlWin::hookAction( HWND hWnd )
{
    if( getMinimizeType() > 0 )
    {
        /*
         * Hide to tray
         */
        hideWindow( hWnd );
    }
}


/*
 *  Minimize a window to the taskbar
 */
void    WindowCtrlWin::minimizeWindowToTaskbar( quint64 window )
{
    ShowWindow( (HWND)window, SW_MINIMIZE );
}


/*
 *  Minimize window to the tray
 */
void    WindowCtrlWin::minimizeWindowToTray( quint64 window )
{
    ShowWindow( (HWND)window, SW_HIDE );

    m_tb_windows_hidden.append( window );

/*
    ShowWindow( (HWND)window, SW_MINIMIZE );

    hideWindow( (HWND)window );
*/
}


/*
 *  Normalize a window
 */
void    WindowCtrlWin::normalizeWindow( quint64 window )
{
    if( !isThunderbird( getPpid() ) )
    {
        return;
    }

    long style = GetWindowLong( (HWND)window, GWL_STYLE );

    style &= ~(WS_EX_TOOLWINDOW);
    style |= WS_EX_APPWINDOW;

    SetWindowLong( (HWND)window, GWL_STYLE, style );

    ShowWindow( (HWND)window, SW_RESTORE );
    SetForegroundWindow( (HWND)window );
}


/*
 *  Normalize a window
 */
void    WindowCtrlWin::normalizeWindowsHidden()
{
    for( int i ; i < m_tb_windows_hidden.length ; ++i )
    {
        ShowWindow( (HWND)m_tb_windows_hidden.hidden.at( i ), SW_SHOW);
    }

    if( m_tb_windows_hidden.length > 0 )
    {
        SetForegroundWindow( (HWND)m_tb_windows_hidden.at( 0 ) );
    }

    m_tb_windows_hidden.clear();
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
 *  Delete / Close a window
 */
void    WindowCtrlWin::deleteWindow( quint64 window )
{
    if( !isThunderbird( getPpid() ) )
    {
        return;
    }

    SendMessageA(  (HWND)window, WM_CLOSE, 0, 0 );
}

#endif // Q_OS_WIN
