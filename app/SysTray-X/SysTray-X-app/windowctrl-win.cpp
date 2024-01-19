#include "windowctrl-win.h"

#ifdef Q_OS_WIN

/*
 *  System includes
 */
#include <TlHelp32.h>
#include <Psapi.h>

/*
 * Qt includes
 */
#include <QCoreApplication>
#include <QString>


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
}


/*
 * Destructor
 */
WindowCtrlWin::~WindowCtrlWin()
{
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
    return getProcessName( pid ).contains( "thunderbird", Qt::CaseInsensitive ) ||
            getProcessName( pid ).contains( "betterbird", Qt::CaseInsensitive );
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
 *  Find the window by pid
 */
void    WindowCtrlWin::findWindows( qint64 pid )
{
    m_tb_windows = QList< quint64 >();
    m_tb_window_states = QMap< quint64, Preferences::WindowState >();

    EnumWindowsPidProcData data{ *this, pid };
    EnumWindows( &enumWindowsPidProc, reinterpret_cast<LPARAM>(&data) );
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
 *  Get the states of the TB windows.
 */
const Preferences::WindowState&    WindowCtrlWin::getWindowState( const quint64 window )
{
    return m_tb_window_states[ window ];
}


/*
 *  Try to match the TB window id to a x11 window
 */
void    WindowCtrlWin::identifyWindow( int id )
{
    /*
     *  Get all the windows connected to TB
     */
    findWindows( getPpid() );

    /*
     *  Get the list
     */
    QList<quint64>  win_list = m_tb_windows;

    /*
     *  Remove known ids
     */
    QMapIterator<int, quint64> it(m_tb_window_refs);
    while (it.hasNext()) {
        it.next();

        int found = win_list.indexOf( it.value() );
        if( found != -1 )
        {
            win_list.removeAt( found );
        }
    }

    /*
     *  Should only one remain
     */
    if( win_list.length() > 0 )
    {
        m_tb_window_refs[ id ] = win_list[ 0 ];
    }

    if( win_list.length() != 1 )
    {
        //        emit signalConsole( QString( "Unexpected Ids: %1" ).arg( win_list.length() ) );
    }
}


/*
 *  Get the Thunderbird window IDs
 */
QList< quint64 >   WindowCtrlWin::getWinIds()
{
    return m_tb_windows;
}


/*
 *  Get the reference IDs
 */
const QMap< int, quint64 >&   WindowCtrlWin::getRefIds() const
{
    return m_tb_window_refs;
}


/*
 *  Get the reference IDs
 */
void    WindowCtrlWin::removeRefId( int id )
{
    m_tb_window_refs.remove( id );
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
 *  Normalize hidden windows
 */
void    WindowCtrlWin::normalizeWindowsHidden()
{
    for( int i = 0 ; i < m_tb_windows_hidden.length() ; ++i )
    {
        ShowWindow( (HWND)m_tb_windows_hidden.at( i ), SW_SHOW );
    }

    if( m_tb_windows_hidden.length() > 0 )
    {
        SetForegroundWindow( (HWND)m_tb_windows_hidden.at( 0 ) );
    }

    m_tb_windows_hidden.clear();
}


/*
 *  Delete / Close a window
 */
void    WindowCtrlWin::deleteHiddenWindows()
{
    for( int i = 0 ; i < m_tb_windows_hidden.length() ; ++i )
    {
        SendMessageA(  (HWND)m_tb_windows_hidden.at( i ), WM_CLOSE, 0, 0 );
    }
}


/*
 *  Delete / Close a window
 */
void    WindowCtrlWin::deleteWindow( quint64 window )
{
    SendMessageA(  (HWND)window, WM_CLOSE, 0, 0 );
}

#endif // Q_OS_WIN
