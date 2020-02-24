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
#include <QRect>

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
        QList< quint64 >   getWinIds();

        /**
         * @brief getWinGeos. Get the window geometries.
         *
         *  @return
         */
        QList< QRect >   getWinGeos();

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



        void    changeStyleWindow( HWND hwnd );

    private:

        /**
         * @brief hideWindow. Hide window.
         *
         *  @param hwnd     The window.
         */
        void    hideWindow( HWND hwnd );

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
        static QList< quint64 >  m_tb_windows;

        /**
         * @brief m_tb_geometries. The Thunderbird window geometries.
         */
        static QList< QRect >  m_tb_geometries;
};

#endif // WINDOWCTRLWIN_H

#endif // Q_OS_WIN
