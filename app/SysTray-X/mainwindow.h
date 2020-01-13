#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/*
 *	Local includes
 */
#include "systrayxlink.h"

/*
 *	Qt includes
 */
#include <QObject>
#include <QMainWindow>



namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private slots:

        /**
         * @brief slotReceivedMessageLength
         *
         * @param msglen
         */
        void    slotReceivedMessageLength( qint32 msglen );

        /**
         * @brief slotReceivedMessage
         *
         * @param message
         */
        void    slotReceivedMessage( QByteArray message );

    signals:

        /**
         * @brief signalWriteMessage
         *
         * @param message
         */
        void    signalWriteMessage( QByteArray message );

    private:
        Ui::MainWindow *ui;

        SysTrayXLink    m_systray_x_link;
};

#endif // MAINWINDOW_H
