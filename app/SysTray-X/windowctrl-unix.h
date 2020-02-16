#include <QtGlobal>
#ifdef Q_OS_UNIX

#ifndef WINDOWCTRLLINUX_H
#define WINDOWCTRLLINUX_H

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



/**
 * @brief The WindowCtrl class.
 */
class WindowCtrl : public QObject
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
         * @brief WindowCtrl. Constructor.
         *
         * @param parent    My parent.
         */
        explicit WindowCtrl( QObject *parent = nullptr );

        /**
         * @brief findWindow. Find window with title.
         *
         *  @param title    The title to find
         */
        void    findWindow( const QString& title );

        /**
         * @brief findWindow. Find window of a process.
         *
         *  @param pid      The process id.
         */
        void    findWindow( int pid );

    private:

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
        QString atomwName( Display *display, Window window );

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

    public slots:

        /**
         * @brief slotWindowState. Handle the window state change signal.
         *
         * @param state     The new state.
         */
        void slotWindowState( QString state );

        /**
         * @brief slotShowHide. Slot for handling of the show / hide window signal.
         */
        void slotShowHide();

        /**
         * @brief slotWindowTest. Start a test.
         */
        void    slotWindowTest();

    private:

        /**
         * @brief m_state. State of the TB window.
         */
        QString m_state;
};

#endif // WINDOWCTRLLINUX_H

#endif // Q_OS_UNIX
