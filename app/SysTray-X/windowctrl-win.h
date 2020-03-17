#include <QtGlobal>
#ifdef Q_OS_WIN

#ifndef WINDOWCTRLWIN_H
#define WINDOWCTRLWIN_H

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
         * @brief getPpid. Get the parent process id.
         *
         *  @return     The ppid
         */
        qint64  getPpid();

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
        bool    findWindow( qint64 pid );

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
         * @brief getWinId. Get the Thunderbird window ID.
         *
         *  @return     The TB window ID.
         */
        quint64 getWinId();

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
        void    minimizeWindow( quint64 window, bool hide );

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
         * @brief m_tb_window. The Thunderbird window.
         */
        quint64  m_tb_window;

        /**
         * @brief m_tb_window. The Thunderbird windows.
         */
        QList< quint64 >  m_tb_windows;
};

#endif // WINDOWCTRLWIN_H

#endif // Q_OS_WIN
