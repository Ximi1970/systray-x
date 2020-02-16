#include <QtGlobal>
#ifdef Q_OS_WIN

#ifndef WINDOWCTRLWIN_H
#define WINDOWCTRLWIN_H

#include <QObject>

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
};

#endif // WINDOWCTRLWIN_H

#endif // Q_OS_WIN
