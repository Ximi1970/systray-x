#include <QtGlobal>
#ifdef Q_OS_UNIX

#ifndef WINDOWCTRLLINUX_H
#define WINDOWCTRLLINUX_H

#include <QObject>

class WindowCtrl : public QObject
{
    Q_OBJECT

    public:

        /**
         * @brief WindowCtrl. Constructor.
         *
         * @param parent    My parent.
         */
        explicit WindowCtrl( QObject *parent = nullptr );

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

    public slots:

        /**
         * @brief slotWindowState. Handle the window state change signal.
         *
         * @param state     The new state.
         */
        void slotWindowState( QString state );

        /**
         * @brief slotShowHide. Slot for handling of the show / hide window signal.
         */
        void slotShowHide();

    private:

        /**
         * @brief m_state. State of the TB window.
         */
        QString m_state;
};

#endif // WINDOWCTRLLINUX_H

#endif // Q_OS_UNIX
