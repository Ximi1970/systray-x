#include <QtGlobal>
#ifdef Q_OS_WIN

#ifndef WINDOWCTRLWIN_H
#define WINDOWCTRLWIN_H

/*
 *  System includes
 */
#include <Windows.h>

/*
 *  Qt includes
 */
#include <QObject>

/*
 *  Define the windows Id
 */
typedef quint64   WinId;

/**
 * @brief The WindowCtrlWin class
 */
class WindowCtrlWin : public QObject
{
    Q_OBJECT

    public:

        /**
         * @brief WindowCtrlWin. Constructor.
         *
         * @param parent    My parent.
         */
        explicit WindowCtrlWin( QObject *parent = nullptr );

        /**
         * @brief findWindow. Find window with title.
         *
         *  @param title    The title to find.
         *
         *  @return     State of the find.
         */
        bool    findWindow( const QString& title );

        /**
         * @brief displayWindowElements. Display window elements.
         *
         *  @param title    The title to find.
         */
        void    displayWindowElements( const QString& title );

        /**
         * @brief getWinIds. Get the Thunderbird window IDs.
         *
         *  @return     The list of window IDs.
         */
        QList< WinId >   getWinIds();

        /**
         * @brief minimizeWindow. Minimize window.
         *
         *  @param window   The window.
         */
        void    minimizeWindow( WinId window );

        /**
         * @brief normalizeWindow. Normalize window.
         *
         *  @param window   The window.
         */
        void    normalizeWindow( WinId window );

        /**
         * @brief hideWindow. Hide window.
         *
         *  @param window   The window.
         *  @param state    The state of the window.
         */
        void    hideWindow( WinId window, bool state );

    private:

        /**
         * @brief EnumWindowsProc. Callback for window enumaration.
         *
         *  @param hwnd         Handle of window.
         *  @param lParam       Argument passed by EnumWindows.
         *
         *  @return     State of callback. (TRUE = continue / FALSE = stop)
         */
        static BOOL CALLBACK  EnumWindowsProc( HWND hwnd, LPARAM lParam );

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
        static QList< WinId >  m_tb_windows;
};

#endif // WINDOWCTRLWIN_H

#endif // Q_OS_WIN
