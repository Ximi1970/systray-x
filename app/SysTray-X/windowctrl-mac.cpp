#include "windowctrl-mac.h"

#ifdef Q_OS_MACOS

/*
 *  System includes
 */
#include <unistd.h>


/*
 *  Constructor
 */
WindowCtrlMac::WindowCtrlMac( QObject *parent ) : QObject( parent )
{
    /*
     *  Initialize
     */
    m_tb_window = 0;
    m_tb_windows = QList< quint64 >();
    m_window_state = Preferences::STATE_UNKNOWN;
}


/*
 *  Set the minimize type
 */
void    WindowCtrlMac::setMinimizeType( Preferences::MinimizeType type )
{
    m_minimize_type = type;
}


/*
 * Set the window state.
 */
void    WindowCtrlMac::setWindowState( int state )
{
    m_window_state = state;
}


/**
 * @brief getWindowState. Get the window state.
 *
 *  @return     The state.
 */
int WindowCtrlMac::getWindowState() const
{
    return m_window_state;
}


/*
 *  Get the minimize type
 */
Preferences::MinimizeType    WindowCtrlMac::getMinimizeType() const
{
    return m_minimize_type;
}


/*
 *  Get the parent pid of SysTray-X, TB hopefully
 */
qint64  WindowCtrlMac::getPpid() const
{
    return getppid();
}


/*
 *  Is the pid from thunderbird
 */
bool    WindowCtrlMac::isThunderbird( qint64 pid ) const
{
    return getProcessName( pid ).contains( "thunderbird", Qt::CaseInsensitive );
}


/*
 *  Get the process name
 */
QString WindowCtrlMac::getProcessName( qint64 pid ) const
{
    QString process_name = QString( "/proc/%1/exe" ).arg( pid );
    QFileInfo process( process_name );

    return process.canonicalFilePath();
}


/*
 *  Find window(s) by title
 */
bool    WindowCtrlMac::findWindow( const QString& title )
{
    return false;
}


/*
 *  Find a window by PID
 */
void    WindowCtrlMac::findWindow( qint64 pid )
{
}


/*
 *  Display window atoms
 */
void    WindowCtrlMac::displayWindowElements( const QString& title )
{
}


/*
 *  Display window atoms
 */
void    WindowCtrlMac::displayWindowElements( quint64 window )
{
}


/*
 *  Get the Thunderbird window ID
 */
quint64 WindowCtrlMac::getWinId()
{
    return m_tb_window;
}


/*
 *  Get the Thunderbird window IDs
 */
QList< quint64 >   WindowCtrlMac::getWinIds()
{
    return m_tb_windows;
}


/*
 *  Minimize a window
 */
void    WindowCtrlMac::minimizeWindow( quint64 window, int hide )
{
    if( !isThunderbird( getPpid() ) )
    {
        return;
    }

    hideWindow( window, hide );

    // minimize() here
}


/*
 *  Hide window to system tray
 */
void    WindowCtrlMac::hideWindow( quint64 window, int set )
{
    hideWindowEvent( window, set );
}


/*
 *  Normalize a window
 */
void    WindowCtrlMac::normalizeWindow( quint64 window )
{
    if( !isThunderbird( getPpid() ) )
    {
        return;
    }

    hideWindow( window, false );

    // normalize here
}


/*
 *  Delete the window
 */
void    WindowCtrlMac::deleteWindow( quint64 window )
{
    if( !isThunderbird( getPpid() ) )
    {
        return;
    }

    // delete here
}


/*
 *  Hide window to system tray
 */
void    WindowCtrlMac::hideWindowEvent( quint64 window, bool set )
{
    if( !isThunderbird( getPpid() ) )
    {
        return;
    }

    if( set )
    {
        // minimize
    }
    else
    {
        // normalize
    }
}

#endif // Q_OS_UNIX
