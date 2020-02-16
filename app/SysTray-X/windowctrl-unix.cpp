#include "windowctrl-unix.h"

#ifdef Q_OS_UNIX

/*
 *  System includes
 */
#include <X11/Xatom.h>

/*
 *  Constructor
 */
WindowCtrlUnix::WindowCtrlUnix( QObject *parent ) : QObject( parent )
{

}


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
 *  Find a window by title
 */
bool    WindowCtrlUnix::findWindow( const QString& title, unsigned long& window )
{
    Display *display = XOpenDisplay( ":0.0" );
    Window rootWindow = XDefaultRootWindow( display );
    QList< WindowItem > windows = listXWindows( display, rootWindow );

    foreach( WindowItem win, windows )
    {
        char *name = nullptr;
        if( XFetchName( display, win.window, &name ) > 0 ) {
            QString win_name( name );

            XFree( name );

            if( win_name.contains( title, Qt::CaseInsensitive ) ) {
                emit signalConsole( QString( "Found: Level %1, XID %2, Name %3" ).arg( win.level ).arg( win.window ).arg( win_name ) );


                QString name = atomName( display, win.window );
                emit signalConsole( QString( "Atom name: %1" ).arg( name ) );


                QStringList types = atomWindowType( display, win.window );
                foreach( QString type, types )
                {
                    emit signalConsole( QString( "Atom type: %1" ).arg( type ) );
                }

                QStringList states = atomState( display, win.window );

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
                    emit signalConsole( "Window State: Unknown" );
                }

                /*
                 *  Return the XID
                 */
                window = win.window;

                return true;
            }
        }
    }

    return false;
}


/*
 *  Find a window by PID
 */
void    WindowCtrlUnix::findWindow( pid_t pid )
{
    Display *display = XOpenDisplay( ":0.0" );
    Window rootWindow = XDefaultRootWindow( display );
    QList< WindowItem > windows = listXWindows( display, rootWindow );

    // Get the PID property atom.
    Atom atom_PID = XInternAtom( display, "_NET_WM_PID", True );
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
        if( Success == XGetWindowProperty( display, win.window, atom_PID, 0, 1, False, XA_CARDINAL,
                                             &type, &format, &nItems, &bytesAfter, &propPID ) )
        {
            if( propPID != nullptr )
            {
                if( pid == *((reinterpret_cast<pid_t *>( propPID ) ) ) )
                {
                    char* name = nullptr;
                    XFetchName( display, win.window, &name );

                    emit signalConsole( QString( "Found: Level %1, XID %2, Name %3" ).arg( win.level ).arg( win.window ).arg( name ) );


                    QString atom_name = atomName( display, win.window );
                    emit signalConsole( QString( "Atom Name %1" ).arg( atom_name ) );

                    QStringList states = atomState( display, win.window );
                    foreach( QString state, states )
                    {
                        emit signalConsole( QString( "Atom state: %1" ).arg( state ) );
                    }

                    QStringList types = atomWindowType( display, win.window );
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
