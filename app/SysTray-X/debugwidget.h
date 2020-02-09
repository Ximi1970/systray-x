#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

/*
 *	Local includes
 */

/*
 *	Qt includes
 */
#include <QWidget>

/*
 *	Predefines
 */
class Preferences;

/*
 *  Namespace
 */
namespace Ui {
    class DebugWidget;
}

/**
 * @brief The DebugWidget class
 */
class DebugWidget : public QWidget
{
    Q_OBJECT

    public:

        /**
         * @brief DebugWidget. Constructor.
         *
         *  @param parent    My parent.
         */
        explicit DebugWidget( Preferences* pref, QWidget* parent = nullptr );

        /**
         * @brief setDebugMessage. Display a debug message.
         *
         *  @param message   The message to display.
         */
        void setDebugMessage( const QString& message );

        /**
         * @brief setRawDataLength. Display the error data length.
         *
         *  @param length    The raw data length.
         */
        void setRawDataLength( int length );

        /**
         * @brief setRswDataMessage. Display the raw data.
         *
         *  @param message   The raw data messsage.
         */
        void setErrorDataMessage( const QString &message );

        /**
         * @brief setUnreadMail. Set the number of unread mails.
         *
         *  @param unread    The number of unread mails.
         */
        void setUnreadMail( int unread );

        /**
         * @brief setError. Set the error message.
         *
         *  @param error     The error message.
         */
        void setError( const QString &error );

    signals:

        /**
         * @brief signalWriteMessage
         *
         *  @param message
         */
        void signalWriteMessage( const QByteArray& message );

    public slots:

        /**
         * @brief slotDebugChange. The debug state changed.
         */
        void slotDebugChange();

        /**
         * @brief slotDebugMessage
         *
         *  @param message
         */
        void slotDebugMessage( const QString& message );

        /**
         * @brief slotReceivedDataLength. The received data length.
         *
         *  @param data_len      The received data length.
         */
        void slotReceivedDataLength( qint32 msglen );

        /**
         * @brief slotReceivedData. The received data.
         *
         *  @param data      The received data.
         */
        void slotReceivedData( const QByteArray& data );

        /**
         * @brief slotSetUnreadMail. Slot for handling unread mail signals.
         *
         *  @param unread_mail  The number of unread mails.
         */
        void slotUnreadMail( int unread_mail );

        /**
         * @brief slotReceiveError. Handle receive error signal.
         *
         *  @param error     Error message.
         */
        void slotReceiveError( const QString& error );

    private:

        /**
         * @brief m_ui. Pointer to the widget.
         */
        Ui::DebugWidget*    m_ui;

        /**
         * @brief m_pref. Pointer to ther preferences.
         */
        Preferences*    m_pref;
};

#endif // DEBUGWIDGET_H
