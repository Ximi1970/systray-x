#include "windowctrl-unix.h"

#ifdef Q_OS_UNIX

/*
 *  System includes
 */
#include <X11/Xatom.h>
#include <X11/Xutil.h>


/*
 *  Constructor
 */
WindowCtrlUnix::WindowCtrlUnix( QObject *parent ) : QObject( parent )
{
    /*
     *  Get the base display and window
     */
    m_display = XOpenDisplay( ":0" );
    m_screen = 0;
    m_root_window = XDefaultRootWindow( m_display );
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

                QString name = atomName( m_display, win.window );
                emit signalConsole( QString( "Atom name: %1" ).arg( name ) );

                QStringList types = atomWindowType( m_display, win.window );
                foreach( QString type, types )
                {
                    emit signalConsole( QString( "Atom type: %1" ).arg( type ) );
                }

                QStringList states = atomState( m_display, win.window );

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
        }
    }
}


/*
 *  Get the Thunderbird window ID
 */
QList< quint64 >   WindowCtrlUnix::getWinIds()
{
    return m_tb_windows;
}


/*
 *  Minimize a window
 */
void    WindowCtrlUnix::minimizeWindow( quint64 window )
{
    XIconifyWindow( m_display, static_cast<Window>( window ), m_screen );
    XFlush( m_display );
}


/*
 *  Normalize a window
 */
void    WindowCtrlUnix::normalizeWindow( quint64 window )
{
//    XMapRaised( m_display, static_cast<Window>( window ) );
    XMapWindow( m_display, static_cast<Window>( window ) );
    XFlush( m_display );
}


/*
 *  Remove window from taskbar
 */
void    WindowCtrlUnix::hideWindow( quint64 window, bool set )
{
    char prop_name[] = "_NET_WM_STATE";
    Atom prop = XInternAtom( m_display, prop_name, True );
    Atom prop_skip_taskbar = XInternAtom( m_display, WindowStates[ STATE_SKIP_TASKBAR ].toUtf8(), True );

    Atom type;
    int format;
    unsigned long remain;
    unsigned long len;
    unsigned char* list = nullptr;

    if( XGetWindowProperty( m_display, static_cast<Window>( window ), prop, 0, sizeof( Atom ), False, XA_ATOM,
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
            XChangeProperty( m_display, static_cast<Window>( window ), prop, XA_ATOM, 32, PropModeAppend, reinterpret_cast<unsigned char*>( &prop_skip_taskbar ), 1 );
        }
        else
        if( !set && present )
        {
            /*
             *  Remove the atom
             */
            XChangeProperty( m_display, static_cast<Window>( window ), prop, XA_ATOM, format, PropModeReplace, reinterpret_cast<unsigned char*>( new_atom_list ), static_cast<int>( len - 1 ) );
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
}







/*
 *  Find a window by PID
 */
void    WindowCtrlUnix::findWindow( pid_t pid )
{
    QList< WindowItem > windows = listXWindows( m_display, m_root_window );

    // Get the PID property atom.
    Atom atom_PID = XInternAtom( m_display, "_NET_WM_PID", True );
    if( atom_PID == None )
    {
        emit signalConsole( QString( "No such atom _NET_WM_PID" ) );
    }

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
                if( pid == *((reinterpret_cast<pid_t *>( propPID ) ) ) )
                {
                    char* name = nullptr;
                    XFetchName( m_display, win.window, &name );

                    emit signalConsole( QString( "Found: Level %1, XID %2, Name %3" ).arg( win.level ).arg( win.window ).arg( name ) );


                    QString atom_name = atomName( m_display, win.window );
                    emit signalConsole( QString( "Atom Name %1" ).arg( atom_name ) );

                    QStringList states = atomState( m_display, win.window );
                    foreach( QString state, states )
                    {
                        emit signalConsole( QString( "Atom state: %1" ).arg( state ) );
                    }

                    QStringList types = atomWindowType( m_display, win.window );
                    foreach( QString type, types )
                    {
                        emit signalConsole( QString( "Atom type: %1" ).arg( type ) );
                    }

                    /*
                     *  Cleanup
                     */
                    if( name != nullptr )
                    {
                        XFree( name );
                    }
                }

                XFree( propPID );
            }
        }
    }
}


#ifdef  RAW_EVENT_SEND

bool        WindowCtrlUnix::generateEvent()
{
    XClientMessageEvent event;
    Atom prop;

    prop = XInternAtom( m_display, "WM_CHANGE_STATE", False );
    if( prop == None )
    {
        return false;
    }

    event.type = ClientMessage;
    event.window = m_tb_window;
    event.message_type = prop;
    event.format = 32;
    event.data.l[0] = IconicState;
    return XSendEvent( m_display, m_root_window, False,
            SubstructureRedirectMask|SubstructureNotifyMask,
                       reinterpret_cast<XEvent *>( &event ) );
}

#endif


/*
 *  Get the X11 window list
 */
QList< WindowCtrlUnix::WindowItem >   WindowCtrlUnix::listXWindows( Display *display, Window window, int level )
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
QString   WindowCtrlUnix::atomName( Display *display, Window window )
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
QStringList    WindowCtrlUnix::atomState( Display *display, Window window )
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
QStringList    WindowCtrlUnix::atomWindowType( Display *display, Window window )
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
