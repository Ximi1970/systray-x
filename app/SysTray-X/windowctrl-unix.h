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

/*
 *	Qt includes
 */
#include <QObject>
#include <QFileInfo>

/*
 *  Predefines
 */
typedef struct _XDisplay Display;

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
         *  State actions
         */
        enum StateActions
        {
            _NET_WM_STATE_REMOVE = 0,
            _NET_WM_STATE_ADD,
            _NET_WM_STATE_TOGGLE
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

                WindowItem( quint64 win, int lev )
                {
                    window = win;
                    level = lev;
                }

                quint64 window;
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
         * @brief setMinimizeType
         *
         *  @param type     Set the minimize type.
         */
        void    setMinimizeType( Preferences::MinimizeType type );

        /**
         * @brief getMinimizeType
         *
         *  @return     The minimize type.
         */
        Preferences::MinimizeType    getMinimizeType() const;

        /**
         * @brief getPpid. Get the parent process id.
         *
         *  @return     The ppid
         */
        qint64  getPpid() const;

        /**
         * @brief isThunderbird. Is this a thunderbird pid.
         *
         *  @param pid  The process Id to check.
         *
         *  @return     True if this is thunderbird.
         */
        bool    isThunderbird( qint64 pid ) const;

        /**
         * @brief getProcessName. Get the name of the proces by pid.
         *
         *  @param pid  The process Id.
         *
         *  @return     The process name.
         */
        QString getProcessName( qint64 pid ) const;

        /**
         * @brief findWindow. Find window by (sub)title.
         *
         *  @param title    The title to find.
         *
         *  @return     State of the find.
         */
        bool    findWindow( const QString& title );

        /**
         * @brief findWindow. Find window of a process.
         *
         *  @param pid      The process id.
         */
        void    findWindow( qint64 pid );

        /**
         * @brief displayWindowElements. Display window elements (atoms).
         *
         *  @param title    The window title to find.
         */
        void    displayWindowElements( const QString& title );

        /**
         * @brief displayWindowElements. Display window elements (atoms).
         *
         *  @param window    The window.
         */
        void    displayWindowElements( quint64 window );

        /**
         * @brief getWinId. Get the Thunderbird window ID.
         *
         *  @return     The window ID.
         */
        quint64 getWinId();

        /**
         * @brief getWinIds. Get the Thunderbird window IDs.
         *
         *  @return     The list of window ID.
         */
        QList< quint64 >    getWinIds();

        /**
         * @brief minimizeWindow. Minimize window.
         *
         *  @param window   The window.
         *  @param hide     Hide from taskbar.
         */
        void    minimizeWindow( quint64 window, int hide );

        /**
         * @brief normalizeWindow. Normalize window.
         *
         *  @param window   The window.
         */
        void    normalizeWindow( quint64 window );

        /**
         * @brief hideWindow. Hide a window from the taskbar.
         *
         *  @param window   The window.
         *  @param set      The state of the window.
         */
        void    hideWindow( quint64 window, int set );

        /**
         * @brief deleteWindow. Delete the window.
         *
         *  @param window   The window.
         */
        void    deleteWindow( quint64 window );

    private:

        /**
         * @brief hideWindowEvent. Hide a window from the taskbar using an event message.
         *
         *  @param window   The window.
         *  @param set      The state of the window.
         */
        void    hideWindowEvent( quint64 window, bool set );

        /**
         * @brief hideWindowAtom. Hide a window from the taskbar editing the atoms.
         *
         *  @param window   The window.
         *  @param set      The state of the window.
         */
        void    hideWindowAtom( quint64 window, bool set );

        /**
         * @brief listXWindows. Get all the windows.
         *
         *  @param display  The display.
         *  @param window   The window.
         *  @param level    The recursion level.
         *
         *  @return     The windows list.
         */
        QList< WindowItem > listXWindows( Display* display, quint64 window, int level = 0 );

        /**
         * @brief sendEvent. Send an event.
         *
         *  @param window    Event target.
         *  @param msg       The message
         *  @param action    The action for the properties
         *  @param prop1     Property 1
         *  @param prop2     Property 2
         *  @param prop3     Property 3
         *  @param prop4     Property 4
         */
        void    sendEvent( quint64 window, const char* msg, long action,
                            long prop1, long prop2 = 0, long prop3 = 0, long prop4 = 0 );

        /**
         * @brief atomwName. Get the title of the window.
         *
         *  @param display  The display
         *  @param window   The window
         *
         *  @return     Name of the window.
         */
        QString atomName( Display* display, quint64 window );

        /**
         * @brief atomNetWmState. Get the _NET_WM_STATE of the window.
         *
         *  @param display  The display
         *  @param window   The window
         *
         *  @return     State of the window.
         */
        QStringList atomNetWmState( Display* display, quint64 window );

        /**
         * @brief atomWmState. Get the WM_STATE of the window.
         *
         *  @param display  The display
         *  @param window   The window
         *
         *  @return     State of the window.
         */
        long    atomWmState( Display* display, quint64 window );

        /**
         * @brief atomType. Get the type of the window.
         *
         *  @param display  The display
         *  @param window   The window
         *
         *  @return     Type of the window.
         */
        QStringList atomWindowType( Display* display, quint64 window );

    signals:

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
        quint64 m_root_window;

        /**
         * @brief m_tb_window. The Thunderbird window.
         */
        quint64 m_tb_window;

        /**
         * @brief m_tb_windows. The Thunderbird windows (used by title search).
         */
        QList< quint64 >    m_tb_windows;

        /**
         * @brief m_minimize_type. Minimize type.
         */
        Preferences::MinimizeType   m_minimize_type;
};

#endif // WINDOWCTRLUNIX_H

#endif // Q_OS_UNIX
