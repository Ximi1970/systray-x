#ifndef SYSTRAY_X_LIB_H
#define SYSTRAY_X_LIB_H

typedef int qint32;                 /* 32 bit signed */
typedef unsigned int quint32;       /* 32 bit unsigned */
typedef unsigned long long quint64; /* 64 bit unsigned */

typedef struct {
    long flags;	/* marks which fields in this structure are defined */
    int x, y;		/* obsolete for new window mgrs, but clients */
    int width, height;	/* should set so old wm's don't mess up */
    int min_width, min_height;
    int max_width, max_height;
        int width_inc, height_inc;
    struct {
        int x;	/* numerator */
        int y;	/* denominator */
    } min_aspect, max_aspect;
    int base_width, base_height;		/* added by ICCCM version 1 */
    int win_gravity;			/* added by ICCCM version 1 */
} SizeHints;

/*
 *  State actions
 */
enum StateActions
{
    _NET_WM_STATE_REMOVE = 0,
    _NET_WM_STATE_ADD,
    _NET_WM_STATE_TOGGLE
};

/*
 *  State atoms
 */
enum StateAtoms
{
    _ATOM_SKIP_TASKBAR  = 0,
    _ATOM_SKIP_PAGER
};


/*
 *  Protocol atoms
 */
enum ProtocolAtoms
{
    _ATOM_DELETE_WINDOW = 0
};

struct Point {
    long x;
    long y;
};


/**
 * @brief OpenDisplay. Open the display.
 *
 *  @return     Pointer to the display struct.
 */
void*   OpenDisplay();

/**
 * @brief GetDefaultRootWindow
 *
 *  @param display   The display
 *
 *  @return     The root window.
 */
quint64 GetDefaultRootWindow( void* display  );

/**
 * @brief GetScreenNumberOfScreen. Get the screen number of the window.
 *
 *  @param display  The display
 *  @param window   The window
 *
 *  @return     The screen number.
 */
int GetScreenNumberOfScreen( void* display, quint64 window );

/**
 * @brief Free. Free allocated X11 resources.
 *
 *  @param free  Pointer to the resource
 */
void    Free( void* free );

/**
 * @brief Flush. Flush the event pipes.
 *
 *  @param display   The display
 */
void    Flush( void* display );

/**
 * @brief Sync
 *
 *  @param display           The display
 */
void    Sync( void* display );

/**
 * @brief QueryTree. Query te window tree.
 *
 *  @param display           The display
 *  @param window            The window
 *  @param root              The root window
 *  @param parent            The parent window
 *  @param children          The list of children
 *  @param childrenCount     The number of children
 *
 *  @return     The status of the query.
 */
int    QueryTree( void* display, quint64 window, quint64* root, quint64* parent, void** children, unsigned int* childrenCount);

/**
 * @brief FetchName. Get the name of the window.
 *
 *  @param display  The display
 *  @param window   The window
 *  @param name     Storage for the name
 *
 *  @return     The status.
 */
int FetchName( void* display, quint64 window, char** name );

/**
 * @brief IconifyWindow. Iconify the window (minimize).
 *
 *  @param display  The display
 *  @param window   The window
 */
void    IconifyWindow( void* display, quint64 window );

/**
 * @brief WithdrawWindow
 *
 *  @param display  The display
 *  @param window   The window
 */
void    WithdrawWindow( void* display, quint64 window );

/**
 * @brief MapWindow. Raise the window to the top.
 *
 *  @param display  The display
 *  @param window   The window
 */
void    MapWindow( void* display, quint64 window );

/**
 * @brief GetWMNormalHints
 *
 *  @param display  The display
 *  @param window   The window
 *  @param hints    Storage for the hints
 */
void    GetWMNormalHints( void* display, quint64 window, SizeHints* hints );

/**
 * @brief SetWMNormalHints
 *
 *  @param display  The display
 *  @param window   The window
 *  @param hints    The hints
 */
void    SetWMNormalHints( void* display, quint64 window, SizeHints hints );

/**
 * @brief MapRaised. Raise the window to the top.
 *
 *  @param display  The display
 *  @param window   The window
 */
void    MapRaised( void* display, quint64 window );

/**
 * @brief SetInputFocus
 *
 *  @param display  The display
 *  @param window   The window
 */
void    SetInputFocus( void* display, quint64 window );

/**
 * @brief GetAtomName. Get the atom name.
 *
 *  @param display  The display
 *  @param atom     The atom.
 *
 *  @return     The atom name.
 */
char*   GetAtomName( void* display, long atom );

/**
 * @brief ChangeWindowTypeProperty. Change the window typw property.
 *
 *  @param display   The display
 *  @param window    The window
 *  @param win_type  The new window type.
 */
void    ChangeWindowTypeProperty( void* display, quint64 window, const char* win_type );

/**
 * @brief GetWindowProperty.  Get a window property.
 *
 *  @param display   The display
 *  @param window    The window
 *  @param atom      The atom to get
 *  @param nlist     Storage for the number of found properties
 *
 *  @return     The properties list
 */
void*   GetWindowProperty( void* display, quint64 window, const char* atom, qint32* nlist );

/**
 * @brief SendEvent. Send an X event.
 *
 *  @param display   The display
 *  @param window    The window
 *  @param msg_type  The name of the atom.
 *  @param prop0     The first optional property of the event
 *  @param prop1     The second optional property of the event
 *  @param prop2     The third optional property of the event
 *  @param prop3     The fourth optional property of the event
 */
void    SendEvent( void* display, quint64 window, const char* msg_type,
                   long prop0 = 0, long prop1 = 0, long prop2 = 0, long prop3 = 0 );

/**
 * @brief GetWindowFrameExtensions. Get the sizes of the window frame extensions.
 *
 *  @param display   The display
 *  @param window    The window
 *  @param left      Storage for the left extension size
 *  @param top       Storage for the top extension size
 *  @param right     Storage for the right extension size
 *  @param bottom    Storage for the bottom extension size
 */
void    GetWindowFrameExtensions( void *display, quint64 window, long* left, long* top, long* right, long* bottom );

/**
 * @brief GetWindowPosition. Get the window position.
 *
 *  @param display   The display
 *  @param window    The window
 *  @param pos_x     Storage for the x coordinate
 *  @param pos_y     Storage for the y coordinate
 */
void    GetWindowPosition( void *display, quint64 window, long* pos_x, long* pos_y );

/**
 * @brief MoveWindow. Set the window position.
 *
 *  @param display   The display
 *  @param window    The window
 *  @param x         The x coordinate
 *  @param y         The y coordinate
 */
void    MoveWindow( void* display, quint64 window, int x, int y );

/**
 * @brief SetErrorHandler. Set the x11 error handler.
 */
void    SetErrorHandler();

/**
 * @brief UnSetErrorHandler. Unset the x11 error handler.
 */
void    UnSetErrorHandler();

#endif // SYSTRAY_X_LIB_H
