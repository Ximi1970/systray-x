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
#include <QMap>
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
         * @brief m_doWork. Status of the worker thread.
         */
        bool	m_doWork;
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
         * @brief sendDisableKdeIntegration. Disable KDE integration.
         */
        void    sendDisableKdeIntegration();

        /**
         * @brief sendPositions
         *
         * @param positions     List of the positions of the TB windows
         */
        void    sendPositions( QMap< quint64, QPoint > positions );

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
        void    signalWindowState( Preferences::WindowState state );

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
         * @brief signalUnreadMail. Signal numder of unread mails.
         *
         * @param unreadMail    The number of unread mails.
         */
        void    signalUnreadMail( int unread_mail );

        /**
         * @brief signalWindowPositions. Signal the startup positions.
         *
         * @param window_positions  The list with positions.
         */
        void    signalWindowPositions( QList< QPoint > window_positions );

    public slots:

        /**
         * @brief slotDebugChange. Handle a change in debug state.
         */
        void    slotDebugChange();

        /**
         * @brief slotMinimizeTypeChange. Slot for handling minimize type change signals.
         */
        void    slotMinimizeTypeChange();

        /**
         * @brief slotMinimizeIconTypeChange. Slot for handling minimize icon type change signals.
         */
        void    slotMinimizeIconTypeChange();

        /**
         * @brief slotStartMinimizedChange. Handle a change in start minimized state.
         */
        void    slotStartMinimizedChange();

        /**
         * @brief slotRestoreWindowPositionsChange. Handle a change in restore window positions state.
         */
        void    slotRestoreWindowPositionsChange();

        /**
         * @brief slotCloseTypeChange. Slot for handling close type change signals.
         */
        void    slotCloseTypeChange();

        /**
         * @brief slotDefaultIconTypeChange. Slot for handling default icon type change signals.
         */
        void    slotDefaultIconTypeChange();

        /**
         * @brief slotDefaultIconDataChange. Slot for handling default icon data change signals.
         */
        void    slotDefaultIconDataChange();

        /**
         * @brief slotHideDefaultIconChange. Slot for handling hide default icon change signals.
         */
        void    slotHideDefaultIconChange();

        /**
         * @brief slotIconTypeChange. Slot for handling icon type change signals.
         */
        void    slotIconTypeChange();

        /**
         * @brief slotIconDataChange. Slot for handling icon data change signals.
         */
        void    slotIconDataChange();

        /**
         * @brief slotShowNumberChange. Handle a change in show number state.
         */
        void    slotShowNumberChange();

        /**
         * @brief slotNumberColorChange. Handle a change in number color.
         */
        void    slotNumberColorChange();

        /**
         * @brief slotNumberSizeChange. Handle a change in number size.
         */
        void    slotNumberSizeChange();

        /**
         * @brief slotNumberAlignmentChange. Handle a change in number alignment.
         */
        void    slotNumberAlignmentChange();

        /**
         * @brief slotNumberMarginsChange. Handle a change in number margins.
         */
        void    slotNumberMarginsChange();

        /**
         * @brief slotCountTypeChange. Slot for handling count type change signals.
         */
        void    slotCountTypeChange();

        /**
         * @brief slotStartupDelayChange. Handle a change in startup delay.
         */
        void    slotStartupDelayChange();

        /**
         * @brief slotPositions. Slot for handling a window positions change.
         */
        void    slotPositions( QMap< quint64,  QPoint > positions );

        /**
         * @brief slotThemeChange. Slot for handling theme change signals.
         */
        void    slotThemeChange();

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
