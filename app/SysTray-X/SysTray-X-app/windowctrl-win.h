#include <QtGlobal>
#ifdef Q_OS_WIN

#ifndef WINDOWCTRLWIN_H
#define WINDOWCTRLWIN_H

/*
 *	Local includes
 */
#include "preferences.h"

/*
 *  System includes
 */
#include <Windows.h>

/*
 *  Standard library includes
 */
#include <string>

/*
 *  Qt includes
 */
#include <QObject>
#include <QMap>

/*
 *  Forward declarations
 */
class WindowCtrlWin;

/**
 * @brief The WindowCtrlWin class
 */
class WindowCtrlWin : public QObject
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

    private:

        struct EnumWindowsPidProcData
        {
            WindowCtrlWin&  window_ctrl;
            long long       pid;
        };

        struct EnumWindowsTitleProcData
        {
            WindowCtrlWin&      window_ctrl;
            const std::string   title;
        };

    public:

        /**
         * @brief WindowCtrlWin. Constructor.
         *
         * @param parent    My parent.
         */
        explicit WindowCtrlWin( QObject *parent = nullptr );

        /**
         * @brief ~WindowCtrlWin. Destructor.
         */
        ~WindowCtrlWin();

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
         * @brief findWindow. Find window by pid.
         *
         *  @param pid  The pid to find.
         */
        void    findWindows( qint64 pid );

        /**
         * @brief getWindowState. Get the state of a TB windows.
         *
         *  @param  window  Window ID.
         *
         *  @return     The window state.
         */
        const Preferences::WindowState&    getWindowState( const quint64 window );

        /**
         * @brief getWinIds. Get the Thunderbird window IDs.
         *
         *  @return     The list of window IDs.
         */
        QList< quint64 >   getWinIds();

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
         * @brief normalizeWindowsHidden. Normalize hidden windows.
         */
        void    normalizeWindowsHidden();

        /**
         * @brief deleteHiddenWindows. Delete all hidden windows.
         */
        void    deleteHiddenWindows();

        /**
         * @brief deleteWindow. Delete window.
         *
         *  @param window   The window.
         */
        void    deleteWindow( quint64 window );

    private:

        /**
         * @brief EnumWindowsPidProc. Callback for window enumaration by pid.
         *
         *  @param hwnd         Handle of window.
         *  @param lParam       Argument passed by EnumWindows.
         *
         *  @return     State of callback. (TRUE = continue / FALSE = stop)
         */
        static BOOL CALLBACK  enumWindowsPidProc( HWND hwnd, LPARAM lParam );

        /**
         * @brief isMainWindow. Check for main window.
         *
         *  @param hwnd     The window handle.
         *
         *  @return     Result of the check.
         */
        static BOOL isMainWindow( HWND hwnd );

    signals:

        /**
         * @brief signalDebugMessage. Signal a debug message.
         *
         * @param message   The message.
         */
        void signalDebugMessage( QString message );

        /**
         * @brief signalWindowNormal. Signal normal window.
         */
        void signalWindowNormal();

        /**
         * @brief signalWindowMinimuze. Signal minimize window.
         */
        void signalWindowMinimize();

        /**
         * @brief signalConsole. Send a console message.
         *
         *  @param message      The message.
         */
        void    signalConsole( QString message );

    private:

        /**
         * @brief m_tb_window. The Thunderbird windows.
         */
        QList< quint64 >  m_tb_windows;

        /**
         * @brief m_tb_windows_hidden. The Thunderbird hidden windows.
         */
        QList< quint64 >  m_tb_windows_hidden;

        /**
         * @brief m_tb_window_states. The Thunderbird window states.
         */
        QMap< quint64, Preferences::WindowState >    m_tb_window_states;

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

        /**
         * @brief m_hook
         */
        HWINEVENTHOOK m_hook;
};

#endif // WINDOWCTRLWIN_H

#endif // Q_OS_WIN
