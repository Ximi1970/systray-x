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
    m_tb_window = 0;
    m_tb_windows = QList< quint64 >();

    /*
     *  Get the base display and window
     */
    m_display = XOpenDisplay( ":0" );
    m_screen = 0;
    m_root_window = XDefaultRootWindow( m_display );
}


/*
 *  Get the parent pid of SysTray-X, TB hopefully
 */
qint64  WindowCtrlUnix::getPpid()
{
    return getppid();
}


/*
 *  Find window(s) by title
 */
bool    WindowCtrlUnix::findWindow( const QString& title )
{
    QList< WindowItem > windows = listXWindows( m_display, m_root_window );

    m_tb_windows = QList< quint64 >();
    foreach( WindowItem win, windows )
    {
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
void    WindowCtrlUnix::findWindow( qint64 pid )
{
    QList< WindowItem > windows = listXWindows( m_display, m_root_window );

    // Get the PID property atom.
    Atom atom_PID = XInternAtom( m_display, "_NET_WM_PID", True );
    if( atom_PID == None )
    {
        return;
    }

    m_tb_window = 0;
    foreach( WindowItem win, windows )
    {
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
                    m_tb_window = win.window;

                    XFree( propPID );
                    return;
                }

                XFree( propPID );
            }
        }
    }
}


/*
 *  Display window atoms
 */
void    WindowCtrlUnix::displayWindowElements( const QString& title )
{
    QList< WindowItem > windows = listXWindows( m_display, m_root_window );

    foreach( WindowItem win, windows )
    {
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
    foreach( QString type, types )
    {
        emit signalConsole( QString( "Atom type: %1" ).arg( type ) );
    }

    QStringList states = atomState( m_display, window );

    bool max_vert = false;
    bool max_horz = false;
    bool hidden = false;

    foreach( QString state, states )
    {
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
 *  Get the Thunderbird window ID
 */
quint64 WindowCtrlUnix::getWinId()
{
    return m_tb_window;
}


/*
 *  Get the Thunderbird window IDs
 */
QList< quint64 >   WindowCtrlUnix::getWinIds()
{
    return m_tb_windows;
}


/*
 *  Minimize a window
 */
void    WindowCtrlUnix::minimizeWindow( quint64 window, bool hide )
{
    if( !window )
    {
        return;
    }

    Window win = static_cast<Window>( window );

    if( hide )
    {
        hideWindow( win, hide );
    }

    XIconifyWindow( m_display, win, m_screen );
    XFlush( m_display );
}


/*
 *  Normalize a window
 */
void    WindowCtrlUnix::normalizeWindow( quint64 window )
{
    if( !window )
    {
        return;
    }

    Window win = static_cast<Window>( window );

    hideWindow( win, false );

    XEvent event = { 0 };
    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.message_type = XInternAtom( m_display, "_NET_ACTIVE_WINDOW", False );
    event.xclient.window = static_cast<Window>( window );
    event.xclient.format = 32;

    XSendEvent( m_display, m_root_window, False, SubstructureRedirectMask | SubstructureNotifyMask, &event );
    XMapRaised( m_display, win );
//    XMapWindow( m_display, win );
    XFlush( m_display );
}


/*
 *  Remove window from taskbar
 */
void    WindowCtrlUnix::hideWindow( quint64 window, bool set )
{
    if( !window )
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
 *  Delete the window
 */
void    WindowCtrlUnix::deleteWindow( quint64 window )
{
    if( !window )
    {
        return;
    }

    Window win = static_cast<Window>( window );

    Atom prop = XInternAtom( m_display, "WM_PROTOCOLS", True );
    if( prop == None )
    {
        return;
    }

    Atom delete_prop = XInternAtom( m_display, "WM_DELETE_WINDOW", False );
    if( prop == None )
    {
        return;
    }

    XEvent event;
    event.xclient.type = ClientMessage;
    event.xclient.window = win;
    event.xclient.message_type = prop;
    event.xclient.format = 32;
    event.xclient.data.l[0] = static_cast<long>( delete_prop );
    event.xclient.data.l[1] = CurrentTime;
    XSendEvent( m_display, win, False, NoEventMask, &event );
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
 *  Get the state of the window
 */
QStringList    WindowCtrlUnix::atomState( Display *display, quint64 window )
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

#endif // Q_OS_UNIX
