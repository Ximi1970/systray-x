#include "systray-x-lib-x11.h"

/*
 *  System includes
 */
#include <stdio.h>
#include <limits.h>
#include <string.h>

/*
 *  X11 includes
 */
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>


/*
 *  X11 error state
 * */
bool    x11Error = false;


/*
 *  Open the display
 */
void*   OpenDisplay()
{
    x11Error = false;

    return XOpenDisplay( NULL );
}


/*
 *  Get the default root window
 */
quint64 GetDefaultRootWindow( void* display  )
{
    return XDefaultRootWindow( (Display*)display );
}


/*
 *  Get the default screen number
 */
int GetDefaultScreen( void* display  )
{
    return XDefaultScreen( (Display*)display );
}


/*
 *  Get the screen number of window
 */
int GetScreenNumberOfScreen( void* display, quint64 window )
{
    XWindowAttributes xwa;
    XGetWindowAttributes( (Display*)display, window, &xwa );
    return XScreenNumberOfScreen( xwa.screen );
}


/*
 *  Free X11 resources
 */
void    Free( void* free )
{
    XFree( free );
}


/*
 *  Flush the event pipes.
 */
void    Flush( void* display )
{
    XFlush( (Display*)display );
}


/*
 *  Sync the event pipes.
 */
void    Sync( void* display )
{
    XSync( (Display*)display, False );
}


/*
 *  Query the window tree
 */
int QueryTree( void* display, quint64 window, quint64* root, quint64* parent, void** children, unsigned int* childrenCount)
{
    Window x11_root;
    Window x11_parent;
    Window* x11_children;
    unsigned int x11_childrenCount;

    int status = XQueryTree( (Display*)display, window, &x11_root, &x11_parent, &x11_children, &x11_childrenCount );

    *root = x11_root;
    *parent = x11_parent;
    *children = x11_children;
    *childrenCount = x11_childrenCount;

    return status;
}


/*
 *  Get the name of a window
 */
int FetchName( void* display, quint64 window, char** name )
{
    return XFetchName( (Display*)display, window, name );
}


/*
 *  Iconify the window ( minimimze )
 */
void    IconifyWindow( void* display, quint64 window )
{
    XIconifyWindow( (Display*)display, window, GetDefaultScreen( display ) );
}


/*
 *  With draw the window
 */
void    WithdrawWindow( void* display, quint64 window )
{
    XWithdrawWindow( (Display*)display, window, GetDefaultScreen( display ) );
}


/*
 *  Map the window
 */
void    MapWindow( void* display, quint64 window )
{
    XMapWindow( (Display*)display, window );
}


/*
 *  Get the hints
 */
void    GetWMNormalHints( void* display, quint64 window, SizeHints* hints )
{
    XSizeHints size_hint;
    long dummy;
    XGetWMNormalHints( (Display*)display, window, &size_hint, &dummy);

    hints->flags = size_hint.flags;
    hints->x = size_hint.x;
    hints->y = size_hint.y;
    hints->width = size_hint.width;
    hints->height = size_hint.height;
    hints->min_width = size_hint.min_width;
    hints->min_height = size_hint.min_height;
    hints->max_width = size_hint.max_width;
    hints->max_height = size_hint.max_height;
    hints->width_inc = size_hint.width_inc;
    hints->height_inc = size_hint.height_inc;
    hints->min_aspect.x = size_hint.min_aspect.x;
    hints->min_aspect.y = size_hint.min_aspect.y;
    hints->max_aspect.x = size_hint.max_aspect.x;
    hints->max_aspect.y = size_hint.max_aspect.y;
    hints->base_width = size_hint.base_width;
    hints->base_height = size_hint.base_height;
    hints->win_gravity = size_hint.win_gravity;
}


/*
 *  Set the hints
 */
void    SetWMNormalHints( void* display, quint64 window, SizeHints hints )
{
    XSizeHints size_hint;

    size_hint.flags = USPosition;
    size_hint.x = hints.x;
    size_hint.y = hints.y;
    size_hint.width = hints.width;
    size_hint.height = hints.height;
    size_hint.min_width = hints.min_width;
    size_hint.min_height = hints.min_height;
    size_hint.max_width = hints.max_width;
    size_hint.max_height = hints.max_height;
    size_hint.width_inc = hints.width_inc;
    size_hint.height_inc = hints.height_inc;
    size_hint.min_aspect.x = hints.min_aspect.x;
    size_hint.min_aspect.y = hints.min_aspect.y;
    size_hint.max_aspect.x = hints.max_aspect.x;
    size_hint.max_aspect.y = hints.max_aspect.y;
    size_hint.base_width = hints.base_width;
    size_hint.base_height = hints.base_height;
    size_hint.win_gravity = hints.win_gravity;

    XSetWMNormalHints( (Display*)display, window, &size_hint );
}


/*
 *  Raise the window to the top
 */
void    MapRaised( void* display, quint64 window )
{
    XMapRaised( (Display*)display, window );
}


/*
 *  Set the input focus
 */
void    SetInputFocus( void* display, quint64 window )
{
    XSetInputFocus( (Display*)display, window, RevertToParent, CurrentTime );
}


/*
 *  Get the name of the atom
 */
char*   GetAtomName( void* display, long atom )
{
    return XGetAtomName( (Display*)display, atom );
}


/*
 *  Change the window type
 */
void    ChangeWindowTypeProperty( void* display, quint64 window, const char* win_type )
{
    Display* dsp = (Display*)display;

    Atom prop = XInternAtom( dsp, "_NET_WM_WINDOW_TYPE", True );
    Atom atom_win_type = XInternAtom( dsp, win_type, True );

    Atom type;
    int format;
    unsigned long remain;
    unsigned long len;
    unsigned char* list = NULL;

    if( XGetWindowProperty( dsp, window, prop, 0, LONG_MAX, False, AnyPropertyType,
                &type, &format, &len, &remain, &list ) == Success && len && list )
    {
        XFree( list );

        XChangeProperty( dsp, window, prop, XA_ATOM, format, PropModeReplace,
                     reinterpret_cast<unsigned char*>( &atom_win_type ), 1 );
    }
}


/*
 *  Get the title of the window
 */
void*   GetWindowProperty( void* display, quint64 window, const char* atom, qint32* nlist )
{
    Display* dsp = (Display*)display;

    Window win;
    if( window == 0 )
    {
        win = XDefaultRootWindow( dsp );
    }
    else
    {
        win = window;
    }

    Atom prop = XInternAtom( (Display*)display, atom, False );

    Atom type;
    int format;
    unsigned long remain;
    unsigned long len;
    unsigned char* list = NULL;

    if( XGetWindowProperty( dsp, win, prop, 0, LONG_MAX, False, AnyPropertyType,
                &type, &format, &len, &remain, &list ) == Success )
    {
        if( nlist != NULL )
        {
            *nlist = (qint32)len;
        }

        return list;
    }

    *nlist = (qint32)-1;

    if( list )
    {
        XFree( list );
    }

    return NULL;
}


/*
 *  Send an X event
 */
void    SendEvent( void* display, quint64 window, const char* msg_type,
                   long prop0, long prop1, long prop2, long prop3 )
{
    Display* dsp = (Display*)display;
    Window root_window = XDefaultRootWindow( dsp );

    Atom msg_atom;
    if( strcmp( msg_type, "WM_PROTOCOLS" ) == 0 )
    {
        msg_atom = XInternAtom( dsp, msg_type, True );
    }
    else
    {
        msg_atom = XInternAtom( dsp, msg_type, False );
    }

    XEvent event;
    event.xclient.type = ClientMessage;
    event.xclient.serial = 0;
    event.xclient.send_event = True;
    event.xclient.message_type = msg_atom;
    event.xclient.window = window;
    event.xclient.format = 32;

    long filter = SubstructureRedirectMask | SubstructureNotifyMask;

    if( strcmp( msg_type, "_NET_WM_STATE" ) == 0 )
    {
        event.xclient.data.l[0] = prop0;
        switch( prop1 )
        {
            case _ATOM_SKIP_TASKBAR:
            {
                event.xclient.data.l[1] = XInternAtom( dsp, "_NET_WM_STATE_SKIP_TASKBAR", False );
                break;
            }

            case _ATOM_SKIP_PAGER:
            {
                event.xclient.data.l[1] = XInternAtom( dsp, "_NET_WM_STATE_SKIP_PAGER", False );
                break;
            }

            default:
            {
                return;
            }
        }
    }
    else
    if( strcmp( msg_type, "WM_PROTOCOLS" ) == 0 )
    {
        switch( prop0 )
        {
            case _ATOM_DELETE_WINDOW:
            {
                event.xclient.data.l[0] = XInternAtom( dsp, "WM_DELETE_WINDOW", False );
                break;
            }

            default:
            {
                return;
            }
        }
        event.xclient.data.l[1] = CurrentTime;

        root_window = window;
        filter = NoEventMask;
    }
    else
    {
        event.xclient.data.l[0] = prop0;
        event.xclient.data.l[1] = prop1;
        event.xclient.data.l[2] = prop2;
        event.xclient.data.l[3] = prop3;
    }

    XSendEvent( dsp, root_window, False, filter, &event );
}


/*
 *  Get the frame extensions
 */
void    GetWindowFrameExtensions( void *display, quint64 window, long* left, long* top, long* right, long* bottom )
{
    Display* dsp = (Display*)display;

    char prop_name[] = "_NET_FRAME_EXTENTS";
    Atom prop = XInternAtom( dsp, prop_name, True );

    /*
     *  Reset the margins
     */
    *left = 0;
    *top = 0;
    *right = 0;
    *bottom = 0;

    /*
     *   Get the frame extentions
     */
    Atom type;
    int format;
    unsigned long remain;
    unsigned long len;
    unsigned char* list = NULL;
    XEvent event;

    while( XGetWindowProperty( dsp, window, prop, 0, 4, False, AnyPropertyType,
                &type, &format, &len, &remain, &list ) != Success || len != 4 || remain != 0 )
    {
        XNextEvent( dsp, &event );
    }

    if( list && len == 4 )
    {
        long* extents = (long*)list;
        *left = extents[ 0 ];
        *right = extents[ 1 ];
        *top = extents[ 2 ];
        *bottom = extents[ 3 ];
    }

    if( list )
    {
        XFree( list );
    }
}


/*
 *  Get the window position
 */
void    GetWindowPosition( void* display, quint64 window, long* pos_x, long* pos_y )
{
    Display* dsp = (Display*)display;

    int x, y;
    Window child;
    XWindowAttributes xwa;
    XTranslateCoordinates( dsp, window, XDefaultRootWindow( dsp ), 0, 0, &x, &y, &child );
    XGetWindowAttributes( dsp, window, &xwa );

    *pos_x = x - xwa.x;
    *pos_y = y - xwa.y;
}


/*
 *  Set the window position
 */
void    MoveWindow( void* display, quint64 window, int x, int y )
{
    XMoveWindow( (Display*)display, window, x, y );
}


/*
 *  The error handler
 */
int ErrorHandler( Display* display, XErrorEvent* event )
{
    fprintf( stderr, "Error code: %x\n", event->error_code );

    char buf[ 1024 ];
    XGetErrorText( display, event->error_code, buf, 1024 );
    fprintf( stderr, "%s\n", buf );

    x11Error = true;
    return 0;
}


/*
 *  Set the error handler
 */
void    SetErrorHandler()
{
    XSetErrorHandler( ErrorHandler );
}


/*
 *  Get the error state
 */
bool    Error()
{
    bool stete = x11Error;
    x11Error = false;

    return stete;
}


/*
 *  Unset the error handler
 */
void    UnSetErrorHandler()
{
    x11Error = false;

    XSetErrorHandler( NULL );
}
