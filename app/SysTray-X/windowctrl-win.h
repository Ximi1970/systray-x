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


        void    closeWindow( HWND hwnd );



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
         * @brief EnumWindowsProc. Callback for window enumaration.
         *
         *  @param hwnd         Handle of window.
         *  @param lParam       Argument passed by EnumWindows.
         *
         *  @return     State of callback. (TRUE = continue / FALSE = stop)
         */
        static BOOL CALLBACK  EnumWindowsProc( HWND hwnd, LPARAM lParam );

        /**
         * @brief WindowProc
         *
         *  @param hwnd
         *  @param uMsg
         *  @param wParam
         *  @param lParam
         *
         *  @return
         */
        static LRESULT CALLBACK WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

        static LRESULT CALLBACK mySubClassProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData );

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
};

#endif // WINDOWCTRLWIN_H

#endif // Q_OS_WIN
