#ifndef WINDOWCTRL_H
#define WINDOWCTRL_H

/*
 *  Qt includes
 */
#include <QtGlobal>

/*
 *  Local includes
 */
#include "options.h"

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
class Container;
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
        explicit WindowCtrl( Container* container, Preferences* pref, QObject *parent = nullptr );

        /**
         * @brief ~WindowCtrlUnix. Destructor.
         */
        ~WindowCtrl();

        /**
         * @brief captureWindow. Capture the TB window.
         *
         *  @param title    (Part of) the window title.
         *
         *  @return     State of the capture.
         */
        bool    captureWindow( const QString& title );

        /**
         * @brief shutdown. Handle app shutdown.
         */
        void    shutdown();

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
         * @brief slotHideOnMinimizeChange. Handle the hide on minimize signal.
         *
         *  @param state    The state
         */
        void    slotHideOnMinimizeChange();

        /**
         * @brief slotMinimizeOnCloseChange. Handle the minimize on close signal.
         *
         *  @param state    The state
         */
        void    slotMinimizeOnCloseChange();

        /**
         * @brief slotWindowState. Handle the window state change signal.
         *
         * @param state     The new state.
         */
        void    slotWindowState( QString state );

        /**
         * @brief slotShowHide. Slot for handling of the show / hide window signal.
         */
        void    slotShowHide();

    private:

        /**
         * @brief m_container. Pointer to the container.
         */
        Container*  m_container;

        /**
         * @brief m_pref. Pointer to the preferences storage.
         */
        Preferences*    m_pref;

        /**
         * @brief m_tb_window. Pointer to the TB window.
         */
        QWindow*    m_tb_window;

        /**
         * @brief m_tb_container. Pointer to the TB widget container.
         */
        QWidget*    m_tb_container;

        /**
         * @brief m_window_title. Title of the TB window.
         */
        QString m_window_title;

        /**
         * @brief m_hide_minimize. State of hide on minimize.
         */
        bool    m_hide_minimize;

        /**
         * @brief m_minimize_close. State of minimize on close.
         */
        bool    m_minimize_close;

        /**
         * @brief m_state. State of the TB window.
         */
        QString m_state;
};


#endif // WINDOWCTRL_H
