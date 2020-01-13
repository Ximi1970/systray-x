#ifndef SYSTRAYXLINK_H
#define SYSTRAYXLINK_H

/*
 *	Local includes
 */

/*
 *	Qt includes
 */
#include <QObject>

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
        SysTrayXLink();
        ~SysTrayXLink();

    public slots:

        /**
         * @brief slotLinkWrite. Write the link.
         */
        void    slotLinkWrite( QByteArray message );

     private slots:

        /**
         * @brief slotLinkRead. Read the link.
         */
        void    slotLinkRead();

        /**
         * @brief slotLinkReadException. Handle a read link exception.
         */
        void    slotLinkReadException();

    signals:

        /**
         * @brief signalReceivedMessageLength
         *
         * @param msglen
         */
        void    signalReceivedMessageLength(qint32 msglen);

        /**
         * @brief signalReceivedMessage
         *
         *  @param message
         */
        void    signalReceivedMessage(QByteArray message);

    private:

        /**
         * @brief m_stdin. Pointer to stdin file.
         */
        QFile *m_stdin;

        /**
         * @brief m_stdin. Pointer to stdout file.
         */
        QFile *m_stdout;

        /**
         * @brief m_notifierLinkRead. Pointers to the link read data notifier.
         */
        QSocketNotifier *m_notifierLinkRead;

        /**
         * @brief m_notifierLinkReadException. Pointers to the link read exception notifier.
         */
        QSocketNotifier *m_notifierLinkReadException;
};

#endif // SYSTRAYXLINK_H
