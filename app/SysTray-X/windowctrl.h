#ifndef WINDOWCTRL_H
#define WINDOWCTRL_H

#include <QtGlobal>

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
         * @brief slotWindowTitle. Handle the window title signal.
         *
         * @param state     The windows title.
         */
        void    slotWindowTitle( QString title );

        /**
         * @brief slotMinimizeTypeChange. Handle the minimize type signal.
         */
        void    slotMinimizeTypeChange();

        /**
         * @brief slotStartMinimizedChange. Handle the start minimized signal.
         */
        void    slotStartMinimizedChange();

        /**
         * @brief slotWindowState. Handle the window state change signal.
         *
         * @param state     The new state.
         */
        void    slotWindowState( int state );

        /**
         * @brief slotShowHide. Slot for handling of the show / hide window signal.
         */
        void    slotShowHide();

        /**
         * @brief slotClose.
         */
        void    slotClose();

    private:

        /**
         * @brief m_pref. Pointer to the preferences storage.
         */
        Preferences*    m_pref;

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
         * @brief m_window_title. Title of the TB window.
         */
        QString m_window_title;

        /**
         * @brief m_start_minimized. State of start minimized.
         */
        bool    m_start_minimized;
};


#endif // WINDOWCTRL_H
