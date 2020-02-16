#include "windowctrl-unix.h"

#ifdef Q_OS_UNIX

//#include <QX11Info>
#include <X11/Xatom.h>

/*
 *  Constructor
 */
WindowCtrl::WindowCtrl(QObject *parent) : QObject(parent)
{

}


void    WindowCtrl::slotWindowTest()
{
    emit signalConsole("Test started");

    // Do something.

    findWindow( "- Mozilla Thunderbird" );

//    findWindow( 4313 );

    emit signalConsole("Test done");
}


/*
 *  Get the X11 window list
 */
QList< WindowCtrl::WindowItem >   WindowCtrl::listXWindows( Display *display, Window window, int level )
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
void    WindowCtrl::findWindow( const QString& title )
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
                    if( max_vert && max_horz )
                    {
                        emit signalConsole( "Window State: Maximize" );
                    }
                    else
                    if( hidden )
                    {
                        emit signalConsole( "Window State: Hidden" );
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
            }
        }
    }
}


/*
 *  Find a window by PID
 */
void    WindowCtrl::findWindow( pid_t pid )
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


                    QString atom_name = atomwName( display, win.window );
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
QString   WindowCtrl::atomwName( Display *display, Window window )
{
    char prop_name[] = "_NET_WM_NAME";
    Atom prop = XInternAtom( display, prop_name, True );

    Atom type;
    int format;
    unsigned long remain;
    unsigned long len;
    unsigned char* list = nullptr;

    QString name;

    if( XGetWindowProperty( display, window, prop, 0, LONG_MAX, False, AnyPropertyType,
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
QStringList    WindowCtrl::atomState( Display *display, Window window )
{
    char prop_name[] = "_NET_WM_STATE";
    Atom prop = XInternAtom( display, prop_name, True );

    Atom type;
    int format;
    unsigned long remain;
    unsigned long len;
    unsigned char* list = nullptr;

    QStringList states;

    if( XGetWindowProperty( display, window, prop, 0, LONG_MAX, False, AnyPropertyType,
                &type, &format, &len, &remain, &list ) == Success )
    {
        for( int i = 0; i < static_cast<int>( len ); ++i )
        {
            Atom tmp_atom = reinterpret_cast<Atom *>( list )[ i ];

            char* atom_name = XGetAtomName( display, tmp_atom );

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
QStringList    WindowCtrl::atomWindowType( Display *display, Window window )
{
    char prop_name[] = "_NET_WM_WINDOW_TYPE";
    Atom prop = XInternAtom( display, prop_name, True );

    Atom type;
    int format;
    unsigned long remain;
    unsigned long len;
    unsigned char* list = nullptr;

    QStringList states;

    if( XGetWindowProperty( display, window, prop, 0, LONG_MAX, False, AnyPropertyType,
                &type, &format, &len, &remain, &list ) == Success )
    {
        for( int i = 0; i < static_cast<int>( len ); ++i )
        {
            Atom tmp_atom = reinterpret_cast<Atom *>( list )[ i ];

            char* atom_name = XGetAtomName( display, tmp_atom );

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
 *  Handle change in window state
 */
void    WindowCtrl::slotWindowState( QString state )
{
    m_state = state;

    emit signalDebugMessage( "Win state: " + state );
}


/*
 *  Handle show / hide signal
 */
void    WindowCtrl::slotShowHide()
{
    if( m_state == "minimized" )
    {
        m_state = "normal";
        emit signalWindowNormal();
    } else {
        m_state = "minimized";
        emit signalWindowMinimize();
    }
}


#endif // Q_OS_UNIX
