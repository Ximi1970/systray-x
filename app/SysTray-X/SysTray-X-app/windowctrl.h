#ifndef WINDOWCTRL_H
#define WINDOWCTRL_H

/*
 *	Qt includes
 */
#include <QtGlobal>


/*
 *	Local includes
 */
#ifdef Q_OS_UNIX
#include "windowctrl-unix.h"
#endif // Q_OS_UNIX

#ifdef Q_OS_WIN
#include "windowctrl-win.h"
#endif // Q_OS_WIN


/*
 *  Predefines
 */
class QWindow;
class Preferences;

/**
 * @brief The WindowCtrl class.
 */
#ifdef Q_OS_UNIX
class WindowCtrl : public WindowCtrlUnix
#elif defined Q_OS_WIN
class WindowCtrl : public WindowCtrlWin
#else
class WindowCtrl : public QObject
#endif
{
    Q_OBJECT

    public:

        /**
         * @brief WindowCtrlUnix. Constructor.
         *
         * @param parent    My parent.
         */
        explicit WindowCtrl( Preferences* pref, QObject *parent = nullptr );

        /**
         * @brief thunderbirdStart. Has the app been started by TB.
         *
         * @return  True if Thunderbird is our parent.
         */
        bool    thunderbirdStart() const;

    signals:

//        void    signalShow();
//        void    signalHide();

        void    signalHideDefaultIconChange(bool hide );

    public slots:

        /**
         * @brief slotWindowTest1. Start a test.
         */
        void    slotWindowTest1();

        /**
         * @brief slotWindowTest2. Start a test.
         */
        void    slotWindowTest2();

        /**
         * @brief slotWindowTest3. Start a test.
         */
        void    slotWindowTest3();

        /**
         * @brief slotWindowTest4. Start a test.
         */
        void    slotWindowTest4();

        /**
         * @brief slotMinimizeTypeChange. Handle the minimize type signal.
         */
        void    slotMinimizeTypeChange();

        /**
         * @brief slotMinimizeIconTypeChange. Handle the minimize icon type signal.
         */
        void    slotMinimizeIconTypeChange();

        /**
         * @brief slotCloseTypeChange. Handle the close type signal.
         */
        void    slotCloseTypeChange();

        /**
         * @brief slotStartMinimizedChange. Handle the start minimized signal.
         */
        void    slotStartMinimizedChange();

        /**
         * @brief slotWindowState. Handle the window state change signal.
         *
         *  @param state    The new state.
         *  @param id       The TB window id.
         */
        void    slotWindowState( Preferences::WindowState state, int id);

        /**
         * @brief slotShowHide. Slot for handling of the show / hide window signal.
         */
        void    slotShowHide();

        /**
         * @brief slotClose.
         */
        void    slotClose();

        /**
         * @brief slotPositions. Slot for handling of the startup window posiions.
         */
        void    slotPositions( QList< QPoint > window_positions );

        /**
         * @brief slotNewWindow. Slot for handling a new window.
         *
         *  @param id   The TB window id.
         */
        void    slotNewWindow( int id );

        /**
         * @brief slotCloseWindow. Slot for handling a close window.
         *
         *  @param id   The TB window id.
         *  @param quit true if the window is really closed.
         */
        void    slotCloseWindow( int id, bool quit );

    private:

        /**
         * @brief m_pref. Pointer to the preferences storage.
         */
        Preferences*    m_pref;

        /**
         * @brief m_show_hide_active
         */
        bool    m_show_hide_active;

        /**
         * @brief m_pid. SysTray-X process pid.
         */
        qint64  m_pid;

        /**
         * @brief m_ppid. SysTray-X parent process pid.
         */
        qint64  m_ppid;

        /**
         * @brief m_tb_window. Pointer to the TB window.
         */
        QWindow*    m_tb_window;

        /**
         * @brief m_start_minimized. State of start minimized.
         */
        bool    m_start_minimized;
};


#endif // WINDOWCTRL_H
