#include <QtGlobal>
#ifdef Q_OS_UNIX

#ifndef WINDOWCTRLUNIX_H
#define WINDOWCTRLUNIX_H

/*
 *	Local includes
 */
#include "preferences.h"

/*
 *  System includes
 */
#include <X11/Xlib.h>

/*
 *	Qt includes
 */
#include <QObject>
#include <QRect>

/**
 * @brief The WindowCtrlUnix class.
 */
class WindowCtrlUnix : public QObject
{
    Q_OBJECT

    public:

        /*
         *  Window types
         */
        enum WindowType
        {
            TYPE_UNKNOWN = 0,
            TYPE_DESKTOP,
            TYPE_DOCK,
            TYPE_TOOLBAR,
            TYPE_MENU,
            TYPE_UTILITY,
            TYPE_SPLASH,
            TYPE_DIALOG,
            TYPE_DROPDOWN_MENU,
            TYPE_POPUP_MENU,
            TYPE_TOOLTIP,
            TYPE_NOTIFICATION,
            TYPE_COMBO,
            TYPE_DND,
            TYPE_NORMAL
        };

        /*
         *  Window states
         */
        enum WindowState
        {
            STATE_MODAL = 0,
            STATE_STICKY,
            STATE_MAXIMIZED_VERT,
            STATE_MAXIMIZED_HORZ,
            STATE_SHADED,
            STATE_SKIP_TASKBAR,
            STATE_SKIP_PAGER,
            STATE_HIDDEN,
            STATE_FULLSCREEN,
            STATE_ABOVE,
            STATE_BELOW,
            STATE_DEMANDS_ATTENTION
        };

        const QStringList WindowStates = {
            "_NET_WM_STATE_MODAL",
            "_NET_WM_STATE_STICKY",
            "_NET_WM_STATE_MAXIMIZED_VERT",
            "_NET_WM_STATE_MAXIMIZED_HORZ",
            "_NET_WM_STATE_SHADED",
            "_NET_WM_STATE_SKIP_TASKBAR",
            "_NET_WM_STATE_SKIP_PAGER",
            "_NET_WM_STATE_HIDDEN",
            "_NET_WM_STATE_FULLSCREEN",
            "_NET_WM_STATE_ABOVE",
            "_NET_WM_STATE_BELOW",
            "_NET_WM_STATE_DEMANDS_ATTENTION"
        };

        /*
         *  Window list item
         */
        class WindowItem
        {
            public:

                WindowItem( Window win, int lev )
                {
                    window = win;
                    level = lev;
                }

                Window  window;
                int     level;
        };

    public:

        /**
         * @brief WindowCtrlUnix. Constructor.
         *
         * @param parent    My parent.
         */
        explicit WindowCtrlUnix( QObject *parent = nullptr );

        /**
         * @brief findWindow. Find window with title.
         *
         *  @param title    The title to find.
         *
         *  @return     State of the find.
         */
        bool    findWindow( const QString& title );

        /**
         * @brief displayWindowElements. Display window elements (atoms).
         *
         *  @param title    The window title to find.
         */
        void    displayWindowElements( const QString& title );

        /**
         * @brief getWinIds. Get the Thunderbird window IDs.
         *
         *  @return     The list of window IDs.
         */
        QList< quint64 >   getWinIds();

        /**
         * @brief getWinGeos. Get the Thunderbird window geometries.
         *
         *  @return     The list of window geometries.
         */
        QList< QRect >   getWinGeos();

        /**
         * @brief minimizeWindow. Minimize window.
         *
         *  @param window   The window.
         *  @param hide     Hide from taskbar.
         */
        void    minimizeWindow( quint64 window, bool hide );

        /**
         * @brief normalizeWindow. Normalize window.
         *
         *  @param window   The window.
         */
        void    normalizeWindow( quint64 window );





        /**
         * @brief findWindow. Find window of a process.
         *
         *  @param pid      The process id.
         */
        void    findWindow( int pid );

        void    setAtomState();

    private:

        /**
         * @brief hideWindow. Hide a window from the taskbar.
         *
         *  @param window   The window.
         *  @param set      The state of the window.
         */
        void    hideWindow( quint64 window, bool set );

        /**
         * @brief listXWindows. Get all the windows.
         *
         *  @param display  The display.
         *  @param window   The window.
         *  @param level    The recursion level.
         *
         *  @return     The windows list.
         */
        QList< WindowItem >   listXWindows( Display* display, Window window, int level = 0 );

        /**
         * @brief atomwName. Get the title of the window.
         *
         *  @param display  The display
         *  @param window   The window
         *
         *  @return     Name of the window.
         */
        QString atomName( Display *display, Window window );

        /**
         * @brief atomState. Get the state of the window.
         *
         *  @param display  The display
         *  @param window   The window
         *
         *  @return     State of the window.
         */
        QStringList    atomState( Display *display, Window window );

        /**
         * @brief atomType. Get the type of the window.
         *
         *  @param display  The display
         *  @param window   The window
         *
         *  @return     Type of the window.
         */
        QStringList    atomWindowType( Display *display, Window window );

    signals:

        /**
         * @brief signalDebugMessage. Signal a debug message.
         *
         * @param message   The message.
         */
        void    signalDebugMessage( QString message );

        /**
         * @brief signalWindowNormal. Signal normal window.
         */
        void    signalWindowNormal();

        /**
         * @brief signalWindowMinimuze. Signal minimize window.
         */
        void    signalWindowMinimize();

        /**
         * @brief signalConsole. Send a console message.
         *
         *  @param message      The message.
         */
        void    signalConsole( QString message );

   private:

        /**
         * @brief m_display. Pointer to the main display.
         */
        Display*    m_display;

        /**
         * @brief m_screen. The screen number.
         */
        int m_screen;

        /**
         * @brief m_root_window. The root window.
         */
        Window  m_root_window;

        /**
         * @brief m_tb_window. The Thunderbird windows.
         */
        QList< quint64 >  m_tb_windows;

        /**
         * @brief m_tb_geometries. The Thunderbird window geometries.
         */
        QList< QRect >  m_tb_geometries;
};

#endif // WINDOWCTRLUNIX_H

#endif // Q_OS_UNIX
