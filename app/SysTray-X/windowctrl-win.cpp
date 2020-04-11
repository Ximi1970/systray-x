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
 *  Constructor
 */
WindowCtrlWin::WindowCtrlWin( QObject *parent) : QObject( parent )
{
    /*
     *  Initialize
     */
    m_tb_window = 0;
    m_tb_windows = QList< quint64 >();
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
 *  Get the parent pid of SysTray-X, TB hopefully
 */
qint64  WindowCtrlWin::getPpid()
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
bool    WindowCtrlWin::isThunderbird( qint64 pid )
{
    return getProcessName( pid ).contains( "thunderbird", Qt::CaseInsensitive );
}


/*
 *  Get the process name
 */
QString WindowCtrlWin::getProcessName( qint64 pid )
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
bool    WindowCtrlWin::findWindow( qint64 pid )
{
    EnumWindowsPidProcData data;
    data.pid = pid;
    data.hwnd = nullptr;
    EnumWindows( &enumWindowsPidProc, reinterpret_cast<LPARAM>(&data) );

    if( data.hwnd == nullptr )
    {
        return false;
    }

    /*
     *  Store it
     */
    m_tb_window = (quint64)data.hwnd;

    return true;
}


/*
 *  Callback for the window enumaration pid find.
 */
BOOL CALLBACK   WindowCtrlWin::enumWindowsPidProc( HWND hwnd, LPARAM lParam )
{
    auto& data = *reinterpret_cast<EnumWindowsPidProcData*>(lParam);
    unsigned long pid = 0;

    GetWindowThreadProcessId( hwnd, &pid );

    if( data.pid != pid || !isMainWindow( hwnd ) )
    {
        return TRUE;
    }

    data.hwnd = hwnd;

    return FALSE;
}


/*
 *  Check for main window
 */
BOOL    WindowCtrlWin::isMainWindow( HWND hwnd )
{
    return GetWindow( hwnd, GW_OWNER ) == nullptr && IsWindowVisible( hwnd );
}


/*
 *  Display the window elements
 */
void    WindowCtrlWin::displayWindowElements( const QString& title )
{
    findWindow( title );

    for( quint64 win_id: getWinIds() )
    {
        emit signalConsole( QString( "Found: XID %1" ).arg( win_id ) );
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
 *  Get the Thunderbird window ID
 */
quint64 WindowCtrlWin::getWinId()
{
    return m_tb_window;
}


/*
 *  Get the Thunderbird window IDs
 */
QList< quint64 >   WindowCtrlWin::getWinIds()
{
    return m_tb_windows;
}


/*
 *  Minimize a window
 */
void    WindowCtrlWin::minimizeWindow( quint64 window, int hide )
{
    if( !isThunderbird( getPpid() ) )
    {
        return;
    }

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
    if( !isThunderbird( getPpid() ) )
    {
        return;
    }

    ShowWindow( (HWND)window, SW_RESTORE );
    SetForegroundWindow( (HWND)window );
}


/*
 *  Hide a window
 */
void    WindowCtrlWin::hideWindow( quint64 window, bool state )
{
    if( !isThunderbird( getPpid() ) )
    {
        return;
    }

    if( state )
    {
        hideWindow( (HWND)window );
    }
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
