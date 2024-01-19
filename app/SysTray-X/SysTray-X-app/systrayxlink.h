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
#include <QList>
#include <QPoint>

/*
 *	Predefines
 */
class QFile;
class QTimer;
class QThread;


/**
 * @brief The SysTrayXLinkReader class. Reader thread.
 */
class SysTrayXLinkReader : public QObject
{
    Q_OBJECT

    public:

        /**
         * @brief Reader. Constructor, destructor.
         */
        SysTrayXLinkReader();
        ~SysTrayXLinkReader();

        /**
         * @brief stopThread. Stop the thread.
         */
        void    stopThread();

    public slots:

        /**
         * @brief startThread. Start the thread.
         */
        void	startThread();

        /**
         * @brief slotWorker. The worker thread started by a "timer".
         */
        void	slotWorker();

    signals:

        /**
         * @brief signalReceivedMessage. Signal the received message.
         *
         *  @param message  The received message.
         */
        void    signalReceivedMessage( QByteArray message );

        /**
         * @brief signalAddOnShutdown. Signal to shutdown the app.
         */
        void    signalErrorAddOnShutdown();

    private:

        /**
         * @brief m_timer. Worker timer.
         */
        QTimer* m_timer;

        /**
         * @brief m_do_work. Status of the worker thread.
         */
        bool	m_do_work;
};


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
        SysTrayXLink( Preferences* pref );
        ~SysTrayXLink();

        /**
         * @brief linkWrite. Write a message to the link.
         *
         *  @param message  Message to be written.
         */
        void    linkWrite( const QByteArray& message );

        /**
         * @brief sendPreferences. Send the preferences to the add-on.
         */
        void    sendPreferences();

        /**
         * @brief sendShutdown. Send shutdown to the add-on.
         */
        void    sendShutdown();

        /**
         * @brief sendOptions. Send the options to the add-on.
         */
        void    sendOptions();

        /**
         * @brief sendPositions
         *
         * @param positions     List of the positions of the TB windows
         */
        void    sendPositions( QList< QPoint > positions );

    private:

        /**
         * @brief MessageDecode. Decode a JSON message.
         *
         * @param message   The message.
         */
        void    DecodeMessage( const QByteArray& message );

        /**
         * @brief DecodePlatform. Decode a JSON platform object.
         *
         * @param platform  The JSON platform.
         */
        void    DecodePlatform( const QJsonObject& platform );

        /**
         * @brief DecodeBrowser. Decode a JSON browser object.
         *
         * @param browser  The JSON browser.
         */
        void    DecodeBrowser( const QJsonObject& browser );

        /**
         * @brief DecodePreferences. Decode a JSON preference object.
         *
         * @param pref  The JSON preferences.
         */
        void    DecodePreferences( const QJsonObject& pref );

        /**
         * @brief DecodeMargins. Decode the margins object.
         *
         * @param margins   The JSON margins.
         *
         * @return  The margins.
         */
        QMargins    DecodeMargins( const QJsonObject& margins );

        /**
         * @brief DecodePositions
         *
         * @param positions     The JSON positions.
         */
        void    DecodePositions( const QJsonArray& positions );

        /**
         * @brief EncodePreferences. Encode the preferences into a JSON document.
         *
         *  @param pref     The preferences.
         */
        void    EncodePreferences( const Preferences& pref );

    signals:

        /**
         * @brief signalConsole. Send a console message.
         *
         *  @param message      The message.
         */
        void    signalConsole( QString message );

        /**
         * @brief signalTitle. Signal the title.
         */
        void    signalTitle( QString title );

        /**
         * @brief signalVersion. Signal the version.
         */
        void    signalVersion( QString version );

        /**
         * @brief signalAddOnShutdown. Signal to shutdown the app.
         */
        void    signalAddOnShutdown();

        /**
         * @brief signalErrorAddOnShutdown. Signal to shutdown the app.
         */
        void    signalErrorAddOnShutdown();

        /**
         * @brief signalWindowState. Signal a change in the window state.
         */
        void    signalWindowState( Preferences::WindowState state, int id );

        /**
         * @brief signalKdeIntegration. Signal KDE integration (and use the KStatusNotifierItem icon)
         *
         *   @param state   The prefered state
         */
        void    signalKdeIntegration( bool state );

        /**
         * @brief signalLocale. Signal the locale.
         */
        void    signalLocale( QString locale );

        /**
         * @brief signalMailCount. Signal numder of unread/new mails.
         *
         * @param unread_mail   The number of unread mails.
         * @param new_mail      The number of new mails.
         */
        void    signalMailCount( int unread_mail, int new_mail );

        /**
         * @brief signalWindowPositions. Signal the startup positions.
         *
         * @param window_positions  The list with positions.
         */
        void    signalWindowPositions( QList< QPoint > window_positions );

        /**
         * @brief signalStartApp. Signal the start app launch request.
         */
        void    signalStartApp();

        /**
         * @brief signalCloseApp. Signal the close app launch request.
         */
        void    signalCloseApp();

        /**
         * @brief signalNewWindow. Signal the new window id.
         *
         *  @param id   TB id of the window.
         */
        void    signalNewWindow( int id );

        /**
         * @brief signalCloseWindow. Signal the close window id.
         *
         *  @param id   TB id of the window.
         *  @param quit true if the window is really closed.
         */
        void    signalCloseWindow( int id, bool quit );

    public slots:

        /**
         * @brief slotPositions. Slot for handling a window positions change.
         */
        void    slotPositions( QList< QPoint > positions );

        /**
         * @brief slotPreferencesChanged
         */
        void    slotPreferencesChanged();

    private slots:

        /**
         * @brief slotLinkRead. Read the link.
         */
        void    slotLinkRead( QByteArray message );

        /**
         * @brief slotErrorAddOnShutdown. Handle shutdown signal from reader thread.
         */
        void    slotErrorAddOnShutdown();

    private:

        /**
         * @brief m_reader_thread. Pointer to the reader thread.
         */
        QThread*    m_reader_thread;

        /**
         * @brief m_pref. Pointer to the preferences storage.
         */
        Preferences*    m_pref;

        /**
         * @brief m_pref_json_doc. Temporary storage for the preferences to be send.
         */
        QJsonDocument   m_pref_json_doc;
};

#endif // SYSTRAYXLINK_H
