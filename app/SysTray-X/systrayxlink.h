#ifndef SYSTRAYXLINK_H
#define SYSTRAYXLINK_H

/*
 *	Local includes
 */
#include "preferences.h"

/*
 *	Qt includes
 */
#include <QObject>
#include <QJsonDocument>

/*
 *	Predefines
 */
class QFile;
class QSocketNotifier;

/**
 * @brief The SysTrayXLink class. Handles the communications link.
 */
class SysTrayXLink : public QObject
{
    Q_OBJECT

    public:

        /**
         * @brief SysTrayXLink. Constructor, destructor.
         */
        SysTrayXLink( Preferences *pref );
        ~SysTrayXLink();

        /**
         * @brief linkWrite. Write a message to the link.
         *
         *  @param message  Message to be written.
         */
        void linkWrite( const QByteArray& message );

        /**
         * @brief sendPreferences. Send the preferences to the add-on.
         */
        void sendPreferences();

    private:

        /**
         * @brief MessageDecode. Decode a JSON message.
         *
         * @param message   The message.
         */
        void DecodeMessage( const QByteArray& message );

        /**
         * @brief DecodePreferences. Decode a JSON preference object.
         *
         * @param pref  The JSON preferences.
         */
        void DecodePreferences( const QJsonObject& pref );

        /**
         * @brief EncodePreferences. Encode the preferences into a JSON document.
         *
         *  @param pref     The preferences.
         */
        void EncodePreferences( const Preferences& pref );

    public slots:

        /**
         * @brief slotDebugChange. Handle a change in debug state.
         */
        void slotDebugChange();

        /**
         * @brief slotLinkWrite. Write the link.
         */
        void slotLinkWrite( QByteArray message );

        /**
         * @brief slotIconTypeChange. Slot for handling icon type change signals.
         */
        void slotIconTypeChange();

        /**
         * @brief slotIconDataChange. Slot for handling icon data change signals.
         */
        void slotIconDataChange();

     private slots:

        /**
         * @brief slotLinkRead. Read the link.
         */
        void slotLinkRead();

        /**
         * @brief slotLinkReadException. Handle a read link exception.
         */
        void slotLinkReadException();

    signals:

        /**
         * @brief signalReceivedMessageLength
         *
         * @param msglen
         */
        void signalReceivedMessageLength( qint32 msglen );

        /**
         * @brief signalReceivedMessage
         *
         *  @param message
         */
        void signalReceivedMessage( QByteArray message );

        /**
         * @brief signalLinkReceiveError. Cannot parse received JSON message.
         *
         * @param error     JSON error message
         */
        void signalLinkReceiveError( QString error );

        /**
         * @brief signalDebugMessage. Signal a debug message.
         *
         * @param message   The message.
         */
        void signalDebugMessage( QString message );

        /**
         * @brief signalUnreadMail. Signal numder of unread mails.
         *
         * @param unreadMail    The number of unread mails.
         */
        void signalUnreadMail( int unread_mail );

    private:

        /**
         * @brief m_pref. Pointer to the preferences storage.
         */
        Preferences *m_pref;

        /**
         * @brief m_stdin. Pointer to stdin file.
         */
        QFile *m_stdin;

        /**
         * @brief m_stdin. Pointer to stdout file.
         */
        QFile *m_stdout;

        /**
         * @brief m_dump. Pointer to dump file.
         */
        QFile *m_dump;

        /**
         * @brief m_notifier_link_read. Pointers to the link read data notifier.
         */
        QSocketNotifier *m_notifier_link_read;

        /**
         * @brief m_notifier_link_read_exception. Pointers to the link read exception notifier.
         */
        QSocketNotifier *m_notifier_link_read_exception;

        /**
         * @brief m_pref_json_doc. Temporary storage for the preferences to be send.
         */
        QJsonDocument m_pref_json_doc;
};

#endif // SYSTRAYXLINK_H
