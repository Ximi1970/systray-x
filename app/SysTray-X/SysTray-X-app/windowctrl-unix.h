#include <QtGlobal>
#ifdef Q_OS_UNIX

#ifndef WINDOWCTRLUNIX_H
#define WINDOWCTRLUNIX_H

/*
 *	Local includes
 */
#include "preferences.h"
#include "systray-x-lib-x11.h"

/*
 *  System includes
 */

/*
 *	Qt includes
 */
#include <QObject>
#include <QMap>
#include <QPoint>

/*
 *  Predefines
 */
class QTimer;

/*
 *  Monitor timeout (ms)
 */
#define STATES_MONITOR_TIMEOUT  500

/**
 * @brief The WindowCtrlUnix class.
 */
class WindowCtrlUnix : public QObject
{
    Q_OBJECT

    public:

        /*
         *  Target types
         */
        enum TargetType
        {
            TYPE_WINDOW_TO_TASKBAR = 0,
            TYPE_WINDOW_TO_SYSTEMTRAY
        };

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
            STATE_NORMAL = -1,
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
         *  Window types
         */
        enum CheckType
        {
            CHECK_MINIMIZE = 1,
            CHECK_NORMALIZE,
            CHECK_ALL,
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
         * @brief disableX11ErrorHandler. Disable the X11 error handler.
         */
        void    disableX11ErrorHandler();

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
         * @brief setMinimizeIconType
         *
         *  @param type     Set the minimize icon type.
         */
        void    setMinimizeIconType( Preferences::MinimizeIconType type );

        /**
         * @brief getMinimizeIconType
         *
         *  @return     The minimize icon type.
         */
        Preferences::MinimizeIconType    getMinimizeIconType() const;

        /**
         * @brief setCloseType
         *
         *  @param type     Set the close type.
         */
        void    setCloseType( Preferences::CloseType type );

        /**
         * @brief getCloseType
         *
         *  @return     The close type.
         */
        Preferences::CloseType    getCloseType() const;

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
         * @brief findWindows. Find all windows of a process.
         *
         *  @param pid      The process id.
         */
        void    findWindows( qint64 pid );

        /**
         * @brief getWinIds. Get the Thunderbird window IDs.
         *
         *  @return     The list of window ID.
         */
        QList< quint64 >    getWinIds();

        /**
         * @brief getWindowState. Get the state of a TB windows.
         *
         *  @param  window  Window ID.
         *
         *  @return     The window state.
         */
        const Preferences::WindowState&    getWindowState( const quint64 window );

        /**
         * @brief getWindowState. Get the state of a TB windows.
         *
         *  @param  window  Window ID.
         *
         *  @return     The window state.
         */
        const Preferences::WindowState&    getWindowStateX11( const quint64 window );

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
         * @brief updatePositions. Update the window positions.
         */
        void    updatePositions();

        /**
         * @brief minimizeWindowToTaskbar. Minimize window to the taskbar.
         *
         *  @param window       The window.
         */
        void    minimizeWindowToTaskbar( quint64 window );

        /**
         * @brief minimizeWindowToTray. Minimize window to the tray.
         *
         *  @param window       The window.
         */
        void    minimizeWindowToTray( quint64 window );

        /**
         * @brief normalizeWindow. Normalize window.
         *
         *  @param window   The window.
         */
        void    normalizeWindow( quint64 window );

        /**
         * @brief deleteWindow. Delete the window.
         *
         *  @param window   The window.
         */
        void    deleteWindow( quint64 window );

        /**
         * @brief setPositions. Set the startup positions.
         *
         * @param window_positions  The window positions list.
         */
        void    setPositions( QList< QPoint > window_positions );

        /**
         * @brief updateX11WindowStates. Update the x11 window states.
         */
        void    updateX11WindowStates( CheckType check_type );

    private:

        /**
         * @brief x11WindowStatesMonitor. Monitor the Thunderbird x11 window states.
         */
        void    x11WindowStatesMonitor();

        /**
         * @brief listXWindows. Get all the windows.
         *
         *  @param display  The display.
         *  @param window   The window.
         *  @param level    The recursion level.
         *
         *  @return     The windows list.
         */
        QList< WindowItem > listXWindows( void* display, quint64 window, int level = 0 );

    signals:

        /**
         * @brief signalConsole. Send a console message.
         *
         *  @param message      The message.
         */
        void    signalConsole( QString message );

        /**
         * @brief signalPositions. Signal the new window position.
         *
         * @param positions
         */
        void    signalPositions( QMap< quint64, QPoint > positions );

   private:

        /**
         * @brief m_display. Pointer to the main display.
         */
        void*    m_display;

        /**
         * @brief m_tb_windows. The Thunderbird windows.
         */
        QList< quint64 >    m_tb_windows;

        /**
         * @brief m_tb_window_states_x11. The Thunderbird window states.
         */
        QMap< quint64, Preferences::WindowState >    m_tb_window_states_x11;

        /**
         * @brief m_tb_window_positions. The Thunderbird window positions.
         */
        QMap< quint64, QPoint >    m_tb_window_positions;

        /**
         * @brief m_tb_window_states. The Thunderbird window states.
         */
        QMap< quint64, Preferences::WindowState >    m_tb_window_states;

        /**
         * @brief m_tb_window_hints. The Thunderbird window hints.
         */
        QMap< quint64, SizeHints >  m_tb_window_hints;

        /**
         * @brief m_minimize_type. Minimize type.
         */
        Preferences::MinimizeType   m_minimize_type;

        /**
         * @brief m_minimize_icon_type. Minimize icon type.
         */
        Preferences::MinimizeIconType   m_minimize_icon_type;

        /**
         * @brief m_close_type. Close type.
         */
        Preferences::CloseType   m_close_type;
};

#endif // WINDOWCTRLUNIX_H

#endif // Q_OS_UNIX
