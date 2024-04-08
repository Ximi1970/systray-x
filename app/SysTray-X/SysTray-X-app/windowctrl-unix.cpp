#include "windowctrl-unix.h"

#ifdef Q_OS_UNIX

/*
 *  Local includes
 */
#include "debug.h"
#include "preferences.h"

/*
 *  System includes
 */
#include <unistd.h>

/*
 *  Qt includes
 */
#include <QApplication>
#include <QTime>
#include <QTimer>
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
#include <QElapsedTimer>
#endif
#include <QFileInfo>


/*
 *  Constructor
 */
WindowCtrlUnix::WindowCtrlUnix( QObject *parent ) : QObject( parent )
{
    /*
     *  Initialize
     */
    m_tb_windows = QList< quint64 >();
    m_tb_window_refs = QMap< int, quint64 >();
    m_tb_window_positions = QMap< quint64, QPoint >();
    m_tb_window_states = QMap< quint64, Preferences::WindowState >();
    m_tb_window_states_x11 = QMap< quint64, QStringList >();
    m_tb_window_hints = QMap< quint64, SizeHints >();

    /*
     *  Set the X11 error handler
     */
    SetErrorHandler();

    /*
     *  Get the base display and window
     */
    m_display = OpenDisplay();
}


/*
 *  Disable te X11 error handler
 */
void    WindowCtrlUnix::disableX11ErrorHandler()
{
    UnSetErrorHandler();
}


/*
 *  Set the minimize type
 */
void    WindowCtrlUnix::setMinimizeType( Preferences::MinimizeType type )
{
    m_minimize_type = type;
}


/*
 *  Get the minimize type
 */
Preferences::MinimizeType    WindowCtrlUnix::getMinimizeType() const
{
    return m_minimize_type;
}


/*
 *  Set the minimize icon type
 */
void    WindowCtrlUnix::setMinimizeIconType( Preferences::MinimizeIconType type )
{
    m_minimize_icon_type = type;
}


/*
 *  Get the minimize icon type
 */
Preferences::MinimizeIconType    WindowCtrlUnix::getMinimizeIconType() const
{
    return m_minimize_icon_type;
}


/*
 *  Set the minimize type
 */
void    WindowCtrlUnix::setCloseType( Preferences::CloseType type )
{
    m_close_type = type;
}


/*
 *  Get the close type
 */
Preferences::CloseType    WindowCtrlUnix::getCloseType() const
{
    return m_close_type;
}


/*
 *  Get the parent pid of SysTray-X, TB hopefully
 */
qint64  WindowCtrlUnix::getPpid() const
{
    return getppid();
}


/*
 *  Is the pid from thunderbird
 */
bool    WindowCtrlUnix::isThunderbird( qint64 pid ) const
{
    return getProcessName( pid ).contains( "thunderbird", Qt::CaseInsensitive ) ||
            getProcessName( pid ).contains( "betterbird", Qt::CaseInsensitive );
}


/*
 *  Get the process name
 */
QString WindowCtrlUnix::getProcessName( qint64 pid ) const
{
    QString process_name = QString( "/proc/%1/exe" ).arg( pid );
    QFileInfo process( process_name );

    return process.canonicalFilePath();
}


/*
 *  Find a window by PID
 */
void    WindowCtrlUnix::findWindows( qint64 pid )
{
#ifdef DEBUG_DISPLAY_ACTIONS
    emit signalConsole( "Find windows" );
#endif

    QList< WindowItem > windows = listXWindows( m_display, GetDefaultRootWindow( m_display ) );

    QMap< quint64, QPoint > old_positions = m_tb_window_positions;

    m_tb_windows = QList< quint64 >();
    m_tb_window_positions = QMap< quint64, QPoint >();
    for( int i = 0 ; i < windows.length() ; ++i )
    {
        WindowItem win = windows.at( i );

        qint32 n_propPID;
        void* propPID = GetWindowProperty( m_display, win.window, "_NET_WM_PID", &n_propPID );

        if( propPID != nullptr )
        {
            if( pid == *((reinterpret_cast<qint64 *>( propPID ) ) ) )
            {
                qint32 n_str;
                void* str_ptr = GetWindowProperty( m_display, win.window, "WM_WINDOW_ROLE", &n_str );

                if( str_ptr != nullptr )
                {
                    if( strcmp( (char*)str_ptr, "3pane") == 0 )
                    {
                        m_tb_windows.append( win.window );

                        QPoint point;
                        if( old_positions.contains( win.window ) )
                        {
                            point = old_positions[ win.window ];
                        }

                        m_tb_window_positions[ win.window ] = point;

                        qint32 n_wm_state;
                        void* wm_state_ptr = GetWindowProperty( m_display, win.window, "WM_STATE", &n_wm_state );

                        Preferences::WindowState win_state = Preferences::STATE_DOCKED;
                        bool add_new_state = true;
                        if( wm_state_ptr != nullptr )
                        {
                            int state = *reinterpret_cast<long *>( wm_state_ptr );

                            switch( state )
                            {
                                case 0:
                                {
                                    /*
                                     *  Docked
                                     */
                                    win_state = Preferences::STATE_DOCKED;

                                    break;
                                }

                                case 1:
                                {
                                    /*
                                     *  Normal
                                     */
                                    win_state = Preferences::STATE_NORMAL;

                                    break;
                                }

                                case 3:
                                {
                                    /*
                                     *  Minimized
                                     */
                                    win_state = Preferences::STATE_MINIMIZED;

                                    break;
                                }

                                default:
                                {
                                    add_new_state = false;

                                    break;
                                }
                            }

                            Free( wm_state_ptr );
                        }

                        if( add_new_state )
                        {
                            m_tb_window_states[ win.window ] = win_state;
                        }
                    }

                    Free( str_ptr );
                }
            }

            Free( propPID );
        }
    }

#ifdef DEBUG_DISPLAY_ACTIONS_DETAILS
    emit signalConsole( QString( "Number of windows found: %1" ).arg( m_tb_windows.length() ) );
#endif

#ifdef DEBUG_DISPLAY_ACTIONS_END
    emit signalConsole( "Find windows done" );
#endif
}


/*
 *  Try to match the TB window id to a x11 window
 */
void    WindowCtrlUnix::identifyWindow( int id )
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
const QList< quint64 >&   WindowCtrlUnix::getWinIds() const
{
    return m_tb_windows;
}


/*
 *  Get the reference IDs
 */
const QMap< int, quint64 >&   WindowCtrlUnix::getRefIds() const
{
    return m_tb_window_refs;
}


/*
 *  Get the reference IDs
 */
void    WindowCtrlUnix::removeRefId( int id )
{
    m_tb_window_refs.remove( id );
}


/*
 *  Get the states of the TB windows.
 */
const Preferences::WindowState&    WindowCtrlUnix::getWindowState( const quint64 window )
{
    return m_tb_window_states[ window ];
}


/*
 *  Get window positions
 */
void    WindowCtrlUnix::updatePositions()
{
#ifdef DEBUG_DISPLAY_ACTIONS
    emit signalConsole( "Update positions" );
#endif

    bool changed = false;
    for( int i = 0 ; i < m_tb_windows.length() ; ++i )
    {
        quint64 window = m_tb_windows.at( i );

        QStringList x11_state = getWindowStateX11( window );
        if( x11_state.contains( "_NET_WM_STATE_MAXIMIZED_VERT" ) &&
                x11_state.contains( "_NET_WM_STATE_MAXIMIZED_HORZ" ) )
        {
            /*
             *  Maximized, skip position store
             */
            continue;
        }

        if( m_tb_window_states[ window ] != Preferences::STATE_MINIMIZED && m_tb_window_states[ window ] != Preferences::STATE_DOCKED )
        {
            /*
             *  Get border / title bar sizes
             */
            int left;
            int top;
            int right;
            int bottom;
            GetWindowFrameExtensions( m_display, window, &left, &top, &right, &bottom );

#ifdef DEBUG_DISPLAY_ACTIONS_DETAILS
            emit signalConsole( QString( "Margins: %1, %2, %3, %4" ).arg( left ).arg( top ).arg( right ).arg( bottom ) );
#endif

            /*
             *  Get the position
             */
            int x;
            int y;
            GetWindowPosition( m_display, window, &x, &y );

            /*
             *  Update the list?
             */
            QPoint point = QPoint( x - left, y - top );

            if( m_tb_window_positions[ window ] != point )
            {
                m_tb_window_positions[ window ] = point;

                /*
                 *  Mar the list changed
                 */
                changed = true;
            }

#ifdef DEBUG_DISPLAY_ACTIONS_DETAILS
            emit signalConsole( QString( "Update pos: %1, %2" ).arg( x - left ).arg( y - top ) );
#endif
        }
    }

    if( changed )
    {
        emit signalPositions( m_tb_window_positions.values() );
    }

#ifdef DEBUG_DISPLAY_ACTIONS_END
    emit signalConsole( "Update positions done" );
#endif
}


/*
 *  Minimize a window to the taskbar
 */
void    WindowCtrlUnix::minimizeWindowToTaskbar( quint64 window )
{
#ifdef DEBUG_DISPLAY_ACTIONS
    emit signalConsole( "Minimize to taskbar" );
#endif

    /*
     *  Store the current window positions
     */
    updatePositions();

    /*
     *  Save the hints
     */
    GetWMNormalHints( m_display, window, &m_tb_window_hints[ window ] );

    /*
     *  Get and store the X11 window state
     */
    m_tb_window_states_x11[ window ] = getWindowStateX11( window );

    /*
     *  Minimize the window
     */
    IconifyWindow( m_display, window );

    /*
     *  Sync the events
     */
    Sync( m_display );

    /*
     *  Store the window state
     */
    m_tb_window_states[ window ] = Preferences::STATE_MINIMIZED;

    /*
     *  Flush the pipes
     */
    Sync( m_display );

#ifdef DEBUG_DISPLAY_ACTIONS_END
    emit signalConsole( "Minimize to taskbar done" );
#endif
}


/*
 *  Minimize window to the tray
 */
void    WindowCtrlUnix::minimizeWindowToTray( quint64 window )
{
#ifdef DEBUG_DISPLAY_ACTIONS
    emit signalConsole( "Minimize to system tray" );
#endif

    /*
     *  Store the current window positions
     */
    updatePositions();

    /*
     *  Save the hints
     */
    GetWMNormalHints( m_display, window, &m_tb_window_hints[ window ] );

    /*
     *  Get and store the X11 window state
     */
    m_tb_window_states_x11[ window ] = getWindowStateX11( window );;

    /*
     *  Set the flags (GNOME, Wayland?)
     */
    SendEvent( m_display, window, "_NET_WM_STATE", _NET_WM_STATE_ADD, _ATOM_SKIP_TASKBAR );
    SendEvent( m_display, window, "_NET_WM_STATE", _NET_WM_STATE_ADD, _ATOM_SKIP_PAGER );

    Flush( m_display );

    /*
     *  Remove from taskbar and task switchers
     */
    WithdrawWindow( m_display, window );

    /*
     *  Store the window state
     */
    m_tb_window_states[ window ] = Preferences::STATE_DOCKED;

    /*
     *  Flush the pipes
     */
    Sync( m_display );

#ifdef DEBUG_DISPLAY_ACTIONS_END
    emit signalConsole( "Minimize to system tray done" );
#endif
}


/*
 *  Normalize a window
 */
void    WindowCtrlUnix::normalizeWindow( quint64 window )
{
#if defined DEBUG_DISPLAY_ACTIONS
    emit signalConsole( "Normalize" );
#endif

    /*
     *  Show window on taskbar and in the switcher
     */
    if( m_tb_window_states[ window ] == Preferences::STATE_DOCKED )
    {
        MapWindow( m_display, window );

        /*
         *  Reset the hide flags
         */
        SendEvent( m_display, window, "_NET_WM_STATE", _NET_WM_STATE_REMOVE, _ATOM_SKIP_TASKBAR );
        SendEvent( m_display, window, "_NET_WM_STATE", _NET_WM_STATE_REMOVE, _ATOM_SKIP_PAGER );

        /*
         *  Was the window maximized?
         */
        if( m_tb_window_states_x11[ window ].contains( "_NET_WM_STATE_MAXIMIZED_VERT" ) &&
                m_tb_window_states_x11[ window ].contains( "_NET_WM_STATE_MAXIMIZED_HORZ" ) )
        {
            SendEvent( m_display, window, "_NET_WM_STATE", _NET_WM_STATE_ADD, _ATOM_MAXIMIZED );
        }

        /*
         * Delete the X11 state
         */
        m_tb_window_states_x11.remove( window );

        /*
         *  Restore the size hints
         */
        SetWMNormalHints( m_display, window, m_tb_window_hints[ window ] );

        Flush( m_display );
    }

    /*
     *  Store the window state
     */
    m_tb_window_states[ window ] = Preferences::STATE_NORMAL;

    /*
     *  Raise the window to the top
     */
    MapRaised( m_display, window );

    /*
     *  Flush the pipes
     */
    Flush( m_display );

    /*
     *  Get the current desktop
     */
    int current_desktop = 0;

    qint32 n_current_desktop;
    long* current_desktop_ptr = (long*)GetWindowProperty( m_display, 0, "_NET_CURRENT_DESKTOP", &n_current_desktop );

    if( current_desktop_ptr != nullptr )
    {
        /*
         *  Store it
         */
        current_desktop = *current_desktop_ptr;

        /*
         *  Cleanup
         */
        Free( current_desktop_ptr );

        /*
         *  Set the desktop for the window
         */
        SendEvent( m_display, window, "_NET_WM_DESKTOP", current_desktop, 1 );
    }

    /*
     *  Normalize
     */
    SendEvent( m_display, window, "_NET_ACTIVE_WINDOW" );

    /*
     *  Flush the pipes
     */
    Sync( m_display );

    /*
     *  Move the window to the last recorded position, seems to be needed for wayland
     */
    QPoint pos = m_tb_window_positions[ window ];
    MoveWindow( m_display, window, pos.x(), pos.y() );
    Flush( m_display );

    /*
     *  Let us wait a bit, maybe this helps...
     */
#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
    QTime state_wait;
#else
    QElapsedTimer state_wait;
#endif
    state_wait.start();

    bool win_normal = false;
    while( state_wait.elapsed() < 1000 && !win_normal )
    {
        /*
         *  Get the WM_STATE
         */
        qint32 n_wm_state;
        void* wm_state_ptr = GetWindowProperty( m_display, window, "WM_STATE", &n_wm_state );

        /*
         *  Get the state
         */
        int state = -1;
        if( wm_state_ptr != nullptr )
        {
            state = *reinterpret_cast<long *>( wm_state_ptr );

            if( state == 1 )
            {
                win_normal = true;
            }

            Free( wm_state_ptr );
        }
    }

#ifdef DEBUG_DISPLAY_ACTIONS_END
    emit signalConsole( "Normalize done" );
#endif
}


/*
 *  Delete the window
 */
void    WindowCtrlUnix::deleteWindow( quint64 window )
{
    SendEvent( m_display, window, "WM_PROTOCOLS", _ATOM_DELETE_WINDOW );

    Flush( m_display );
}


/*
 *  Set the window positions
 */
void    WindowCtrlUnix::setPositions( QList< QPoint > window_positions )
{
#ifdef DEBUG_DISPLAY_ACTIONS
    emit signalConsole( "Set positions" );
#endif

    for( int i = 0 ; i < m_tb_windows.length() ; ++i )
    {
        quint64 window = m_tb_windows.at( i );

#ifdef DEBUG_DISPLAY_ACTIONS_DETAILS
        emit signalConsole( QString( "Set pos: %1, %2").arg( window_positions.at( i ).x() ).arg( window_positions.at( i ).y() ) );
#endif

        if( i < window_positions.length() ) {
            MoveWindow( m_display, window, window_positions.at( i ).x(), window_positions.at( i ).y() );
        }
    }

    Flush( m_display );

#ifdef DEBUG_DISPLAY_ACTIONS
    emit signalConsole( "Set positions done" );
#endif
}


/*
 *  Get the X11 window list
 */
QList< WindowCtrlUnix::WindowItem >   WindowCtrlUnix::listXWindows( void* display, quint64 window, int level )
{
    quint64 root;
    quint64 parent;
    void* children;
    unsigned int childrenCount;

    QList< WindowItem > windows;
    if( QueryTree( display, window, &root, &parent, &children, &childrenCount) )
    {
        for( unsigned int i = 0; i < childrenCount; ++i )
        {
            windows.append( WindowItem( ((quint64*)children)[ i ], level ) );
            windows.append( listXWindows( display, ((quint64*)children)[ i ], level + 1) );
        }

        Free( children );
    }

    return windows;
}


/*
 *  Get the window state from X11
 */
QStringList WindowCtrlUnix::getWindowStateX11( quint64 window )
{
    qint32 n_net_wm_state;
    void* net_wm_state_ptr = GetWindowProperty( m_display, window, "_NET_WM_STATE", &n_net_wm_state );

    /*
     *  Get the atoms
     */
    QStringList atom_list;
    if( net_wm_state_ptr != nullptr )
    {
        for( qint32 i = 0 ; i < n_net_wm_state ; ++i )
        {
             char* atom_name = GetAtomName( m_display, reinterpret_cast<long *>( net_wm_state_ptr )[ i ] );

             atom_list.append( atom_name );

             if( atom_name )
             {
                 Free( atom_name );
             }
        }

        Free( net_wm_state_ptr );
    }
/*
    for( int i = 0 ; i < atom_list.length() ; ++i )
     {
         emit signalConsole( QString( "Atom: %1").arg( atom_list.at( i ) ) );
     }
*/
    return atom_list;
}

#endif // Q_OS_UNIX
