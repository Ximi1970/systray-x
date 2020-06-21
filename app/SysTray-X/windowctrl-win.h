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

    private:

        struct EnumWindowsPidProcData
        {
            unsigned long   pid;
            HWND            hwnd;
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
         * @brief findWindow. Find window by title.
         *
         *  @param title    The (part)title to find.
         *
         *  @return     State of the find.
         */
        bool    findWindow( const QString& title );

        /**
         * @brief findWindow. Find window by pid.
         *
         *  @param pid  The pid to find.
         *
         *  @return     State of the find.
         */
        bool    findWindows( qint64 pid );

        /**
         * @brief displayWindowElements. Display window elements.
         *
         *  @param title    The title to find.
         */
        void    displayWindowElements( const QString& title );

        /**
         * @brief displayWindowElements. Display window elements.
         *
         *  @param title    The window id.
         */
        void    displayWindowElements( quint64 window );

        /**
         * @brief getWinIds. Get the Thunderbird window IDs.
         *
         *  @return     The list of window IDs.
         */
        QList< quint64 >   getWinIds();

        /**
         * @brief minimizeWindow. Minimize window.
         *
         *  @param window   The window.
         *  @param hide     Hide from taskbar
         */
        void    minimizeWindow( quint64 window, int hide );

        /**
         * @brief normalizeWindow. Normalize window.
         *
         *  @param window   The window.
         */
        void    normalizeWindow( quint64 window );

        /**
         * @brief WindowCtrlWin::hideWindow
         *
         *  @param window   The window.
         *  @param state    Hide from taskbar.
         */
        void    hideWindow( quint64 window , bool state );

        /**
         * @brief deleteWindow. Delete window.
         *
         *  @param window   The window.
         */
        void    deleteWindow( quint64 window );

    private:

        /**
         * @brief hideWindow. Hide window.
         *
         *  @param hwnd     The window.
         */
        void    hideWindow( HWND hwnd );

        /**
         * @brief EnumWindowsTitleProc. Callback for window enumaration by title.
         *
         *  @param hwnd         Handle of window.
         *  @param lParam       Argument passed by EnumWindows.
         *
         *  @return     State of callback. (TRUE = continue / FALSE = stop)
         */
        static BOOL CALLBACK  enumWindowsTitleProc( HWND hwnd, LPARAM lParam );

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

        /**
         * @brief interceptMinimizeWindow. Intercept minimize event of the TB window.
         */
        void    interceptMinimizeWindow();

        /**
         * @brief stopInterceptMinimizeWindow. Remove the intercept hook.
         */
        void    stopInterceptMinimizeWindow();

        /**
         * @brief handleWinEvent. Callback for the hook.
         *
         *  @param hook
         *  @param event
         *  @param hwnd
         *  @param idObject
         *  @param idChild
         *  @param dwEventThread
         *  @param dwmsEventTime
         */
        static void CALLBACK    handleWinEvent( HWINEVENTHOOK hook, DWORD event, HWND hwnd,
                                                    LONG idObject, LONG idChild,
                                                    DWORD dwEventThread, DWORD dwmsEventTime);

        /**
         * @brief hookAction. Non-static function to be used by the hook callback.
         */
        void    hookAction();

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

    protected:

        /**
         * @brief m_intercept
         */
        static WindowCtrlWin*  m_ctrl_parent;

    private:

        /**
         * @brief m_tb_window. The Thunderbird windows.
         */
        QList< quint64 >  m_tb_windows;

        /**
         * @brief m_minimize_type. Minimize type.
         */
        Preferences::MinimizeType   m_minimize_type;

        /**
         * @brief m_hook
         */
        HWINEVENTHOOK m_hook;
};

#endif // WINDOWCTRLWIN_H

#endif // Q_OS_WIN
