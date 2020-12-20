#include "windowctrl-unix.h"

#ifdef Q_OS_UNIX

/*
 *  System includes
 */
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>


/*
 *  Constructor
 */
WindowCtrlUnix::WindowCtrlUnix( QObject *parent ) : QObject( parent )
{
    /*
     *  Initialize
     */
    m_tb_windows = QList< quint64 >();
    m_tb_windows_pos = QList< QPoint >();
    m_tb_window_states = QList< Preferences::WindowState >();;

    /*
     *  Get the base display and window
     */
    m_display = XOpenDisplay( NULL );
    m_screen = 0;
    m_root_window = XDefaultRootWindow( m_display );
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
    return getProcessName( pid ).contains( "thunderbird", Qt::CaseInsensitive );
}


/*
 *  Get the number of visible windows.
 */
int WindowCtrlUnix::getVisibleWindows()
{
    /*
     *  Get the TB windows and states
     */
    findWindows( getppid() );

    /*
     *  Count the visible states
     */
    return m_tb_window_states.count( Preferences::STATE_NORMAL );
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
 *  Find window(s) by title
 */
bool    WindowCtrlUnix::findWindow( const QString& title )
{
    QList< WindowItem > windows = listXWindows( m_display, m_root_window );

    m_tb_windows = QList< quint64 >();
    for( int i = 0 ; i < windows.length() ; ++i )
    {
        WindowItem win = windows.at( i );

        char *name = nullptr;
        if( XFetchName( m_display, win.window, &name ) > 0 ) {
            QString win_name( name );

            XFree( name );

            if( win_name.contains( title, Qt::CaseInsensitive ) ) {
                /*
                 *  Store the XID
                 */
                m_tb_windows.append( static_cast<quint64>( win.window ) );
            }
        }
    }

    if( m_tb_windows.length() > 0 )
    {
        return true;
    }

    return false;
}


/*
 *  Find a window by PID
 */
void    WindowCtrlUnix::findWindows( qint64 pid )
{
    QList< WindowItem > windows = listXWindows( m_display, m_root_window );

    // Get the PID property atom.
    Atom atom_PID = XInternAtom( m_display, "_NET_WM_PID", True );
    if( atom_PID == None )
    {
        return;
    }

    m_tb_windows = QList< quint64 >();
    for( int i = 0 ; i < windows.length() ; ++i )
    {
        WindowItem win = windows.at( i );

        Atom           type;
        int            format;
        unsigned long  nItems;
        unsigned long  bytesAfter;

        unsigned char* propPID = nullptr;
        if( Success == XGetWindowProperty( m_display, win.window, atom_PID, 0, 1, False, XA_CARDINAL,
                                             &type, &format, &nItems, &bytesAfter, &propPID ) )
        {
            if( propPID != nullptr )
            {
                if( pid == *((reinterpret_cast<qint64 *>( propPID ) ) ) )
                {
                    long state = atomWmState( m_display, win.window );

                    if( state > 0 )
                    {
                        m_tb_windows.append( win.window );
                    }
                }

                XFree( propPID );
            }
        }
    }

//    emit signalConsole( QString( "Number of windows found: %1" ).arg( m_tb_windows.length() ) );

    /*
     *  Get the new window states, store the old ones
     */
    m_tb_window_states = QList< Preferences::WindowState >();
    for( int i = 0 ; i< m_tb_windows.length() ; ++i )
    {
        QStringList atom_list = atomNetWmState( m_display, m_tb_windows.at( i ) );

//        emit signalConsole( QString( "WinID %1, Atoms: %2" ).arg( m_tb_windows.at( i ) ).arg( atom_list.join(",") ) );

        if( atom_list.contains( "_NET_WM_STATE_HIDDEN" ) )
        {
            m_tb_window_states.append( Preferences::STATE_MINIMIZED );
        }
        else
        {
            m_tb_window_states.append( Preferences::STATE_NORMAL );
        }
    }
}


/*
 *  Get the states of the TB windows.
 */
const QList< Preferences::WindowState >&    WindowCtrlUnix::getWindowStates() const
{
    return m_tb_window_states;
}


/*
 *  Display window atoms
 */
void    WindowCtrlUnix::displayWindowElements( const QString& title )
{
    QList< WindowItem > windows = listXWindows( m_display, m_root_window );

    for( int i = 0 ; i < windows.length() ; ++i )
    {
        WindowItem win = windows.at( i );

        char *name = nullptr;
        if( XFetchName( m_display, win.window, &name ) > 0 ) {
            QString win_name( name );

            XFree( name );

            if( win_name.contains( title, Qt::CaseInsensitive ) ) {

                emit signalConsole( QString( "Found: Level %1, XID %2, Name %3" ).arg( win.level ).arg( win.window ).arg( win_name ) );

                displayWindowElements( win.window );
            }
        }
    }
}


/*
 *  Display window atoms
 */
void    WindowCtrlUnix::displayWindowElements( quint64 window )
{
    QString name = atomName( m_display, window );
    emit signalConsole( QString( "Atom name: %1" ).arg( name ) );

    QStringList types = atomWindowType( m_display, window );
    for( int i = 0 ; i < types.length() ; ++i )
    {
        emit signalConsole( QString( "Atom type: %1" ).arg( types.at( i ) ) );
    }

    QStringList states = atomNetWmState( m_display, window );

    bool max_vert = false;
    bool max_horz = false;
    bool hidden = false;

    for( int i = 0 ; i < states.length() ; ++i )
    {
        QString state = states.at( i );

        emit signalConsole( QString( "Atom state: %1" ).arg( state ) );

        int state_code = WindowStates.indexOf( state ) ;

        switch( state_code )
        {
            case STATE_MAXIMIZED_VERT:
            {
                max_vert = true;
                break;
            }

            case STATE_MAXIMIZED_HORZ:
            {
                max_horz = true;
                break;
            }

            case STATE_HIDDEN:
            {
                hidden = true;
                break;
            }
        }
    }

    if( states.length() > 0 )
    {
        if( hidden )
        {
            emit signalConsole( "Window State: Hidden" );
        }
        else
        if( max_vert && max_horz )
        {
            emit signalConsole( "Window State: Maximize" );
        }
        else
        {
            emit signalConsole( "Window State: Normal" );
        }
    }
    else
    {
        emit signalConsole( "Window State: Normal" );
    }
}


/*
 *  Get the Thunderbird window IDs
 */
QList< quint64 >   WindowCtrlUnix::getWinIds()
{
    return m_tb_windows;
}


/*
 *  Get window positions
 */
void    WindowCtrlUnix::updatePositions()
{
    QList< QPoint > positions;
    for( int i = 0 ; i < m_tb_windows.length() ; ++i )
    {
        quint64 window = m_tb_windows.at( i );

        //  Get border / title bar sizes
        QMargins margin = atomFrameExtents( m_display, window );

        int x, y;
        Window child;
        XWindowAttributes xwa;
        Window win = static_cast<Window>( window );
        XTranslateCoordinates( m_display, win, m_root_window, 0, 0, &x, &y, &child );
        XGetWindowAttributes( m_display, win, &xwa );

        positions.append( QPoint( x - xwa.x - margin.left(), y - xwa.y - margin.top() ) );
    }

    if( m_tb_windows_pos != positions )
    {
        m_tb_windows_pos = positions;

        emit signalPositions( m_tb_windows_pos );
    }
}


/*
 *  Minimize a window
 */
void    WindowCtrlUnix::minimizeWindow( quint64 window, int hide )
{
    emit signalConsole( "Minimize" );

    if( !isThunderbird( getPpid() ) )
    {
        emit signalConsole( "Minimize terminated" );

        return;
    }

    hideWindow( window, hide );

    XIconifyWindow( m_display, static_cast<Window>( window ), m_screen );
    XFlush( m_display );
}


/*
 *  Hide window to system tray
 */
void    WindowCtrlUnix::hideWindow( quint64 window, int set )
{
    emit signalConsole( "Hide" );

    switch( m_minimize_type )
    {
        case Preferences::PREF_MINIMIZE_METHOD_1:
        {
            emit signalConsole( "Hide 1" );

            hideWindowEvent( window, set );
            break;
        }

        case Preferences::PREF_MINIMIZE_METHOD_2:
        {
            emit signalConsole( "Hide 2" );

            hideWindowAtom( window, set );
            break;
        }

        default:
        {
            emit signalConsole( "Hide default" );
            break;
        }
    }
}


/*
 *  Normalize a window
 */
void    WindowCtrlUnix::normalizeWindow( quint64 window )
{
    emit signalConsole( "Normalize" );

    if( !isThunderbird( getPpid() ) )
    {
        emit signalConsole( "Normalize terminated" );

        return;
    }

    hideWindow( window, false );

    Window win = static_cast<Window>( window );

    Atom msg_atom = XInternAtom( m_display, "_NET_ACTIVE_WINDOW", False );
    if( msg_atom == None )
    {
        return;
    }

    XEvent event = { 0 };
    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.message_type = msg_atom;
    event.xclient.window = win;
    event.xclient.format = 32;

    XSendEvent( m_display, m_root_window, False, SubstructureRedirectMask | SubstructureNotifyMask, &event );

    XMapRaised( m_display, win );
//    XMapWindow( m_display, win );
    XFlush( m_display );
}


/*
 *  Delete the window
 */
void    WindowCtrlUnix::deleteWindow( quint64 window )
{
    if( !isThunderbird( getPpid() ) )
    {
        return;
    }

    Window win = static_cast<Window>( window );

    Atom msg_atom = XInternAtom( m_display, "WM_PROTOCOLS", True );
    if( msg_atom == None )
    {
        return;
    }

    Atom delete_prop = XInternAtom( m_display, "WM_DELETE_WINDOW", False );
    if( delete_prop == None )
    {
        return;
    }

    XEvent event;
    event.xclient.type = ClientMessage;
    event.xclient.window = win;
    event.xclient.message_type = msg_atom;
    event.xclient.format = 32;
    event.xclient.data.l[0] = static_cast<long>( delete_prop );
    event.xclient.data.l[1] = CurrentTime;
    XSendEvent( m_display, win, False, NoEventMask, &event );
    XFlush( m_display );
}


/*
 *  Set the window positions
 */
void    WindowCtrlUnix::setPositions( QList< QPoint > window_positions )
{
    for( int i = 0 ; i < m_tb_windows.length() ; ++i )
    {
        quint64 window = m_tb_windows.at( i );

        if( i < window_positions.length() ) {
            XMoveWindow( m_display, window, window_positions.at( i ).x(), window_positions.at( i ).y() );
        }
    }

    XFlush( m_display );
}


/*
 *  Hide window to system tray
 */
void    WindowCtrlUnix::hideWindowEvent( quint64 window, bool set )
{
    if( !isThunderbird( getPpid() ) )
    {
        return;
    }

    if( set )
    {
        emit signalConsole( "Hide remove from taskbar" );

        sendEvent( window,
                    "_NET_WM_STATE",
                    _NET_WM_STATE_ADD,
                    static_cast<long>( XInternAtom( m_display, "_NET_WM_STATE_SKIP_TASKBAR", False ) ) );
    }
    else
    {
        emit signalConsole( "Show on taskbar" );

        sendEvent( window,
                    "_NET_WM_STATE",
                    _NET_WM_STATE_REMOVE,
                    static_cast<long>( XInternAtom( m_display, "_NET_WM_STATE_SKIP_TASKBAR", False ) ) );
    }

    XFlush( m_display );
}


/*
 *  Remove window from taskbar
 */
void    WindowCtrlUnix::hideWindowAtom( quint64 window, bool set )
{
    if( !isThunderbird( getPpid() ) )
    {
        return;
    }

    Window win = static_cast<Window>( window );

    char prop_name[] = "_NET_WM_STATE";
    Atom prop = XInternAtom( m_display, prop_name, True );
    Atom prop_skip_taskbar = XInternAtom( m_display, WindowStates[ STATE_SKIP_TASKBAR ].toUtf8(), True );

    Atom type;
    int format;
    unsigned long remain;
    unsigned long len;
    unsigned char* list = nullptr;

    if( XGetWindowProperty( m_display, win, prop, 0, sizeof( Atom ), False, XA_ATOM,
                &type, &format, &len, &remain, &list ) == Success )
    {
        Atom* atom_list = reinterpret_cast<Atom *>( list );
        Atom* new_atom_list = nullptr;
        bool present = false;

        if( len > 1 )
        {
            /*
             *  Check and remove atom from list
             */
            new_atom_list = new Atom[ len - 1 ];

            for( unsigned long i = 0, o = 0; i < len; ++i )
            {
                if( atom_list[ i ] == prop_skip_taskbar )
                {
                    present = true;
                    continue;
                }

                new_atom_list[ o++ ] = atom_list[ i ];
            }
        }

        if( set && !present  )
        {
            /*
             *  Set the atom
             */
            XChangeProperty( m_display, win, prop, XA_ATOM, 32, PropModeAppend,
                             reinterpret_cast<unsigned char*>( &prop_skip_taskbar ), 1 );
        }
        else
        if( !set && present )
        {
            /*
             *  Remove the atom
             */
            XChangeProperty( m_display, win, prop, XA_ATOM, format, PropModeReplace,
                             reinterpret_cast<unsigned char*>( new_atom_list ), static_cast<int>( len - 1 ) );
        }

        /*
         *  Cleanup
         */
        if( new_atom_list )
        {
            delete [] new_atom_list;
        }
    }

    /*
     *  Cleanup
     */
    if( list )
    {
        XFree( list );
    }

    XFlush( m_display );
}


/*
 *  Get the X11 window list
 */
QList< WindowCtrlUnix::WindowItem >   WindowCtrlUnix::listXWindows( Display *display, quint64 window, int level )
{
    Window root;
    Window parent;
    Window *children;
    unsigned int childrenCount;

    QList< WindowItem > windows;
    if( XQueryTree( display, window, &root, &parent, &children, &childrenCount) )
    {
        for( unsigned int i = 0; i < childrenCount; ++i )
        {
            windows.append( WindowItem( children[ i ], level ) );
            windows.append( listXWindows( display, children[ i ], level + 1) );
        }

        XFree( children );
    }

    return windows;
}


/*
 *  Send a X event
 */
void    WindowCtrlUnix::sendEvent( quint64 window, const char* msg, long action,
                                    long prop1, long prop2, long prop3, long prop4 )
{
    Window win = static_cast<Window>( window );

    Atom msg_atom = XInternAtom( m_display, msg, False );
    if( msg_atom == None )
    {
        return;
    }

    XEvent event;
    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.message_type = msg_atom;
    event.xclient.window = win;
    event.xclient.format = 32;
    event.xclient.data.l[0] = action;
    event.xclient.data.l[1] = prop1;
    event.xclient.data.l[2] = prop2;
    event.xclient.data.l[3] = prop3;
    event.xclient.data.l[4] = prop4;

    XSendEvent( m_display, DefaultRootWindow( m_display ), False, SubstructureRedirectMask | SubstructureNotifyMask, &event );
}


/*
 *  Get the title of the window
 */
QString   WindowCtrlUnix::atomName( Display *display, quint64 window )
{
    char prop_name[] = "_NET_WM_NAME";
    Atom prop = XInternAtom( display, prop_name, True );
    Atom utf8_string = XInternAtom( display, "UTF8_STRING", False );

    Atom type;
    int format;
    unsigned long remain;
    unsigned long len;
    unsigned char* list = nullptr;

    QString name;

    if( XGetWindowProperty( display, window, prop, 0, LONG_MAX, False, utf8_string,
                &type, &format, &len, &remain, &list ) == Success )
    {
        name = QString( reinterpret_cast<char*>( list ) );
    }

    if( list )
    {
        XFree( list );
    }

    return name;
}


/*
 *  Get the _NET_WM_STATE of the window
 */
QStringList    WindowCtrlUnix::atomNetWmState( Display *display, quint64 window )
{
    char prop_name[] = "_NET_WM_STATE";
    Atom prop = XInternAtom( display, prop_name, True );

    Atom type;
    int format;
    unsigned long remain;
    unsigned long len;
    unsigned char* list = nullptr;

    QStringList states;

//    if( XGetWindowProperty( display, window, prop, 0, LONG_MAX, False, AnyPropertyType,
//                &type, &format, &len, &remain, &list ) == Success )
    if( XGetWindowProperty( display, window, prop, 0, sizeof( Atom ), False, XA_ATOM,
                &type, &format, &len, &remain, &list ) == Success )
    {
        for( unsigned long i = 0; i < len; ++i )
        {
            char* atom_name = XGetAtomName( display, reinterpret_cast<Atom *>( list )[ i ] );

            states.append( atom_name );

            if( atom_name )
            {
                XFree( atom_name );
            }
        }
    }

    if( list )
    {
        XFree( list );
    }

    return states;
}


/*
 *  Get the WM_STATE of the window
 */
long   WindowCtrlUnix::atomWmState( Display *display, quint64 window )
{
    char prop_name[] = "WM_STATE";
    Atom prop = XInternAtom( display, prop_name, False );

    Atom type;
    int format;
    unsigned long remain;
    unsigned long len = 0;
    unsigned char* data = nullptr;

    long state = -1;
    if( XGetWindowProperty( display, window, prop, 0L, 2L, False, prop,
                &type, &format, &len, &remain, &data ) == Success && len )
    {
        state = *reinterpret_cast<long *>( data );
    }

    if( data )
    {
        XFree( data );
    }

    return state;
}


/*
 *  Get the type of the window
 */
QStringList    WindowCtrlUnix::atomWindowType( Display *display, quint64 window )
{
    char prop_name[] = "_NET_WM_WINDOW_TYPE";
    Atom prop = XInternAtom( display, prop_name, True );

    Atom type;
    int format;
    unsigned long remain;
    unsigned long len;
    unsigned char* list = nullptr;

    QStringList states;

    if( XGetWindowProperty( display, window, prop, 0, sizeof( Atom ), False, XA_ATOM,
                &type, &format, &len, &remain, &list ) == Success )
    {
        for( unsigned long i = 0; i < len; ++i )
        {
            char* atom_name = XGetAtomName( display, reinterpret_cast<Atom *>( list )[ i ] );

            states.append( atom_name );

            if( atom_name )
            {
                XFree( atom_name );
            }
        }
    }

    if( list )
    {
        XFree( list );
    }

    return states;
}


/*
 *  Get the title of the window
 */
QMargins   WindowCtrlUnix::atomFrameExtents( Display *display, quint64 window )
{
    char prop_name[] = "_NET_FRAME_EXTENTS";
    Atom prop = XInternAtom( display, prop_name, True );

    Atom type;
    int format;
    unsigned long remain;
    unsigned long len;
    unsigned char* list = nullptr;
    XEvent event;

    //  Get the frame extentions
    while( XGetWindowProperty( display, window, prop, 0, 4, False, AnyPropertyType,
                &type, &format, &len, &remain, &list ) != Success || len != 4 || remain != 0 )
    {
        XNextEvent( display, &event );
    }

    QMargins margins;
    if( list && len == 4 )
    {
        long* extents = (long*)list;
        margins.setLeft( extents[ 0 ] );
        margins.setRight( extents[ 1 ] );
        margins.setTop( extents[ 2 ] );
        margins.setBottom( extents[ 3 ] );
    }

    if( list )
    {
        XFree( list );
    }

    return margins;
}


#endif // Q_OS_UNIX
