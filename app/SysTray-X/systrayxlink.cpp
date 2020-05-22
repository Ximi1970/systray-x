#include "systrayxlink.h"


/*
 *	Local includes
 */
#include "preferences.h"


/*
 *  System includes
 */
#include <iostream>
#ifdef Q_OS_WIN
#include <fcntl.h>
#include <io.h>
#endif


/*
 *	Qt includes
 */
#include <QFile>
#include <QTimer>
#include <QString>
#include <QThread>
#include <QVariant>
#include <QJsonValue>
#include <QJsonObject>


/*****************************************************************************
 *
 *  SysTrayXLinkReader Class
 *
 *****************************************************************************/


/*
 *	Constructor
 */
SysTrayXLinkReader::SysTrayXLinkReader()
{
    /*
     *  Set stdin to binary
     */
#ifdef Q_OS_WIN
    _setmode( _fileno( stdin ), _O_BINARY);
#endif

    /*
     *	Setup the timer
     */
    m_timer = new QTimer( this );
    m_timer->setSingleShot( true );
    connect( m_timer, &QTimer::timeout, this, &SysTrayXLinkReader::slotWorker );
}


/*
 *	Destructor
 */
SysTrayXLinkReader::~SysTrayXLinkReader()
{
    /*
     *  Cleanup
     */
    m_timer->stop();
    delete m_timer;
}


/*
 *	Start reading
 */
void    SysTrayXLinkReader::startThread()
{
    /*
     *	Start the work
     */
    m_doWork = true;

    /*
     *	Start the worker
     */
    m_timer->start();
}


/*
 *	Stop viewing
 */
void    SysTrayXLinkReader::stopThread()
{
    /*
     *	Stop working
     */
    m_doWork = false;
}


/*
 *	Read the data
 */
void    SysTrayXLinkReader::slotWorker()
{
    int error_count = 0;

    while( m_doWork )
    {
        qint32 data_len;
        std::cin.read( reinterpret_cast< char* >( &data_len ), sizeof( qint32 ) );

        if( data_len > 0)
        {
            QByteArray data( data_len, 0 );
            std::cin.read( data.data(), data_len );

            /*
             *  Send the data to my parent
             */
            emit signalReceivedMessage( data );

            /*
             *  Send the data to my parent
             */
            if( data.at( 0 ) == '{' )
            {
                emit signalReceivedMessage( data );

                error_count = 0;
            }
            else
            {
                error_count++;

                if( error_count > 20 )
                {
 //                   emit signalAddOnShutdown();
                }
            }
        }
    }

    /*
     *	Quit this thread
     */
    QThread::currentThread()->quit();
}


/*****************************************************************************
 *
 *  SysTrayXLink Class
 *
 *****************************************************************************/


/*
 *	Constructor
 */
SysTrayXLink::SysTrayXLink( Preferences* pref )
{
    /*
     *  Store preferences
     */
    m_pref = pref;

    /*
     *  Open dump.txt
     */
//    m_dump = new QFile( "dump.txt", this );
//    m_dump->open( QIODevice::WriteOnly );

    /*
     *  Setup the reader thread
     */
    m_reader_thread = new QThread( this );

    SysTrayXLinkReader* reader = new SysTrayXLinkReader;
    reader->moveToThread( m_reader_thread );

    connect( m_reader_thread, &QThread::finished, reader, &QObject::deleteLater );
    connect( reader, &SysTrayXLinkReader::signalReceivedMessage, this, &SysTrayXLink::slotLinkRead );
    connect( reader, &SysTrayXLinkReader::signalAddOnShutdown, this, &SysTrayXLink::slotAddOnShutdown );

    connect( m_reader_thread, &QThread::started, reader, &SysTrayXLinkReader::startThread, Qt::QueuedConnection );
    m_reader_thread->start();
}


/*
 *	Destructor
 */
SysTrayXLink::~SysTrayXLink()
{
    /*
     *  Cleanup
     */
//    m_dump->close();
//    delete m_dump;
}


/*
 *  Write a message to the link
 */
void    SysTrayXLink::linkWrite( const QByteArray& message )
{
    qint32  msglen = message.length();
    std::cout.write( reinterpret_cast< char* >( &msglen ), sizeof( qint32 ) );
    std::cout.write( message.data(), msglen ) << std::flush;
}


/*
 *  Send the preferences to the add-on
 */
void    SysTrayXLink::sendPreferences()
{
    /*
     *  Enacode the preferences into a JSON doc
     */
    EncodePreferences( *m_pref );

    /*
     *  Send them to the add-on
     */
    linkWrite( m_pref_json_doc.toJson( QJsonDocument::Compact ) );
}


/*
 *  Send shutdown to the add-on
 */
void    SysTrayXLink::sendShutdown()
{
    QJsonObject shutdownObject;
    shutdownObject.insert("shutdown", QJsonValue::fromVariant( "true" ) );

    /*
     *  Store the new document
     */
    QJsonDocument json_doc = QJsonDocument( shutdownObject );

    /*
     *  Send it to the add-on
     */
    linkWrite( json_doc.toJson( QJsonDocument::Compact ) );
}


/*
 *  Decode JSON message
 */
void    SysTrayXLink::DecodeMessage( const QByteArray& message )
{
    QJsonParseError jsonError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson( message, &jsonError );

    if( jsonError.error == QJsonParseError::NoError )
    {
        QJsonObject jsonObject = jsonResponse.object();

        if( jsonObject.contains( "unreadMail" ) && jsonObject[ "unreadMail" ].isDouble() )
        {
            int unreadMail = jsonObject[ "unreadMail" ].toInt();
            emit signalUnreadMail( unreadMail );
        }

        if( jsonObject.contains( "title" ) && jsonObject[ "title" ].isString() )
        {
            QString title = jsonObject[ "title" ].toString();
            emit signalTitle( title );
        }

        if( jsonObject.contains( "version" ) && jsonObject[ "version" ].isString() )
        {
            QString version = jsonObject[ "version" ].toString();
            emit signalVersion( version );
        }

        if( jsonObject.contains( "shutdown" ) && jsonObject[ "shutdown" ].isString() )
        {
            emit signalAddOnShutdown();
        }

        if( jsonObject.contains( "window" ) && jsonObject[ "window" ].isString() )
        {
            QString window_state_str = jsonObject[ "window" ].toString();


            emit signalConsole( QString( "Window state (%1)" ).arg( window_state_str ) );


            int window_state;
            if( window_state_str == Preferences::STATE_NORMAL_STR )
            {
                window_state = Preferences::STATE_NORMAL;
            }
            else
            if( window_state_str == Preferences::STATE_MINIMIZED_STR )
            {
                window_state = Preferences::STATE_MINIMIZED;
            }
            else
            if( window_state_str == Preferences::STATE_MAXIMIZED_STR )
            {
                window_state = Preferences::STATE_MAXIMIZED;
            }
            else
            if( window_state_str == Preferences::STATE_FULLSCREEN_STR )
            {
                window_state = Preferences::STATE_FULLSCREEN;
            }
            else
            if( window_state_str == Preferences::STATE_DOCKED_STR )
            {
                window_state = Preferences::STATE_DOCKED;
            }
            else
            {
                /*
                 *  Unknown state
                 */
                emit signalConsole( QString( "Error: unknow window state (%1)" ).arg( window_state_str ) );

                window_state = Preferences::STATE_NORMAL;
            }

            emit signalWindowState( window_state );
        }

        if( jsonObject.contains( "platformInfo" ) && jsonObject[ "platformInfo" ].isObject() )
        {
            DecodePlatform( jsonObject[ "platformInfo" ].toObject() );
        }

        if( jsonObject.contains( "browserInfo" ) && jsonObject[ "browserInfo" ].isObject() )
        {
            DecodeBrowser( jsonObject[ "browserInfo" ].toObject() );
        }

        if( jsonObject.contains( "preferences" ) && jsonObject[ "preferences" ].isObject() )
        {
            DecodePreferences( jsonObject[ "preferences" ].toObject() );
        }
    }
}


/*
 *  Decode platform from JSON message
 */
void    SysTrayXLink::DecodePlatform( const QJsonObject& platform )
{
    /*
     *  Check the received object
     */
    if( platform.contains( "os" ) && platform[ "os" ].isString() )
    {
        QString os = platform[ "os" ].toString();

        /*
         *  Store the os
         */
        m_pref->setPlatformOs( os );
    }

    if( platform.contains( "arch" ) && platform[ "arch" ].isString() )
    {
        QString arch = platform[ "arch" ].toString();

        /*
         *  Store the arch
         */
        m_pref->setPlatformArch( arch );
    }

    if( platform.contains( "nacl_arch" ) && platform[ "nacl_arch" ].isString() )
    {
        QString nacl_arch = platform[ "nacl_arch" ].toString();

        /*
         *  Store the nacl_arch
         */
        m_pref->setPlatformNaclArch( nacl_arch );
    }
}


/*
 *  Decode platform from JSON message
 */
void    SysTrayXLink::DecodeBrowser( const QJsonObject& browser )
{
    /*
     *  Check the received object
     */
    if( browser.contains( "name" ) && browser[ "name" ].isString() )
    {
        QString name = browser[ "name" ].toString();

        /*
         *  Store the name
         */
        m_pref->setBrowserName( name );
    }

    if( browser.contains( "vendor" ) && browser[ "vendor" ].isString() )
    {
        QString vendor = browser[ "vendor" ].toString();

        /*
         *  Store the vendor
         */
        m_pref->setBrowserVendor( vendor );
    }

    if( browser.contains( "version" ) && browser[ "version" ].isString() )
    {
        QString version = browser[ "version" ].toString();

        /*
         *  Store the version
         */
        m_pref->setBrowserVersion( version );
    }

    if( browser.contains( "buildID" ) && browser[ "buildID" ].isString() )
    {
        QString buildID = browser[ "buildID" ].toString();

        /*
         *  Store the buildID
         */
        m_pref->setBrowserBuildID( buildID );
    }
}


/*
 *  Decode preferences from JSON message
 */
void    SysTrayXLink::DecodePreferences( const QJsonObject& pref )
{
    /*
     *  Check the received object
     */
    if( pref.contains( "defaultIconType" ) && pref[ "defaultIconType" ].isString() )
    {
        Preferences::DefaultIconType icon_type = static_cast< Preferences::DefaultIconType >( pref[ "defaultIconType" ].toString().toInt() );

        /*
         *  Store the new default icon type
         */
        m_pref->setDefaultIconType( icon_type );
    }

    if( pref.contains( "defaultIconMime" ) && pref[ "defaultIconMime" ].isString() )
    {
        QString icon_mime = pref[ "defaultIconMime" ].toString();

        /*
         *  Store the new icon mime
         */
        m_pref->setDefaultIconMime( icon_mime );
    }

    if( pref.contains( "defaultIcon" ) && pref[ "defaultIcon" ].isString() )
    {
        QString icon_base64 = pref[ "defaultIcon" ].toString();

        /*
         *  Store the new default icon data
         */
        m_pref->setDefaultIconData( QByteArray::fromBase64( icon_base64.toUtf8() ) );
    }

    if( pref.contains( "iconType" ) && pref[ "iconType" ].isString() )
    {
        Preferences::IconType icon_type = static_cast< Preferences::IconType >( pref[ "iconType" ].toString().toInt() );

        /*
         *  Store the new icon type
         */
        m_pref->setIconType( icon_type );
    }

    if( pref.contains( "iconMime" ) && pref[ "iconMime" ].isString() )
    {
        QString icon_mime = pref[ "iconMime" ].toString();

        /*
         *  Store the new icon mime
         */
        m_pref->setIconMime( icon_mime );
    }

    if( pref.contains( "icon" ) && pref[ "icon" ].isString() )
    {
        QString icon_base64 = pref[ "icon" ].toString();

        /*
         *  Store the new icon data
         */
        m_pref->setIconData( QByteArray::fromBase64( icon_base64.toUtf8() ) );
    }
    if( pref.contains( "showNumber" ) && pref[ "showNumber" ].isString() )
    {
        bool show_number = pref[ "showNumber" ].toString() == "true";

        /*
         *  Store the new show number state
         */
        m_pref->setShowNumber( show_number );
    }

    if( pref.contains( "numberColor" ) && pref[ "numberColor" ].isString() )
    {
        QString number_color = pref[ "numberColor" ].toString();

        /*
         *  Store the new number color
         */
        m_pref->setNumberColor( number_color );
    }

    if( pref.contains( "countType" ) && pref[ "countType" ].isString() )
    {
        Preferences::CountType count_type = static_cast< Preferences::CountType >( pref[ "countType" ].toString().toInt() );

        /*
         *  Store the new count type
         */
        m_pref->setCountType( count_type );
    }

    if( pref.contains( "minimizeType" ) && pref[ "minimizeType" ].isString() )
    {
        Preferences::MinimizeType minimize_type = static_cast< Preferences::MinimizeType >( pref[ "minimizeType" ].toString().toInt() );

        /*
         *  Store the new minimize type
         */
        m_pref->setMinimizeType( minimize_type );
    }

    if( pref.contains( "startMinimized" ) && pref[ "startMinimized" ].isString() )
    {
        bool start_minimized = pref[ "startMinimized" ].toString() == "true";

        /*
         *  Store the new start minimized state
         */
        m_pref->setStartMinimized( start_minimized );
    }

    if( pref.contains( "minimizeOnClose" ) && pref[ "minimizeOnClose" ].isString() )
    {
        bool minimize_on_close = pref[ "minimizeOnClose" ].toString() == "true";

        /*
         *  Store the new start minimized state
         */
        m_pref->setMinimizeOnClose( minimize_on_close );
    }

    if( pref.contains( "debug" ) && pref[ "debug" ].isString() )
    {
        bool debug = pref[ "debug" ].toString() == "true";

        /*
         *  Store the new debug state
         */
        m_pref->setDebug( debug );
    }
}


/*
 *  Encode preferences to JSON message
 */
void    SysTrayXLink::EncodePreferences( const Preferences& pref )
{
    /*
     *  Setup the preferences JSON
     */
    QJsonObject prefObject;
    prefObject.insert("debug", QJsonValue::fromVariant( QString( pref.getDebug() ? "true" : "false" ) ) );
    prefObject.insert("minimizeType", QJsonValue::fromVariant( QString::number( pref.getMinimizeType() ) ) );
    prefObject.insert("startMinimized", QJsonValue::fromVariant( QString( pref.getStartMinimized() ? "true" : "false" ) ) );
    prefObject.insert("minimizeOnClose", QJsonValue::fromVariant( QString( pref.getMinimizeOnClose() ? "true" : "false" ) ) );
    prefObject.insert("defaultIconType", QJsonValue::fromVariant( QString::number( pref.getDefaultIconType() ) ) );
    prefObject.insert("defaultIconMime", QJsonValue::fromVariant( pref.getDefaultIconMime() ) );
    prefObject.insert("defaultIcon", QJsonValue::fromVariant( QString( pref.getDefaultIconData().toBase64() ) ) );
    prefObject.insert("iconType", QJsonValue::fromVariant( QString::number( pref.getIconType() ) ) );
    prefObject.insert("iconMime", QJsonValue::fromVariant( pref.getIconMime() ) );
    prefObject.insert("icon", QJsonValue::fromVariant( QString( pref.getIconData().toBase64() ) ) );
    prefObject.insert("showNumber", QJsonValue::fromVariant( QString( pref.getShowNumber() ? "true" : "false" ) ) );
    prefObject.insert("numberColor", QJsonValue::fromVariant( QString( pref.getNumberColor() ) ) );
    prefObject.insert("countType", QJsonValue::fromVariant( QString::number( pref.getCountType() ) ) );

    QJsonObject preferencesObject;
    preferencesObject.insert("preferences", prefObject );

    /*
     *  Store the new document
     */
    m_pref_json_doc = QJsonDocument( preferencesObject );
}


/*
 *  Relay shutdown signal
 */
void    SysTrayXLink::slotAddOnShutdown()
{
    emit signalAddOnShutdown();
}


/*
 *  Read the input
 */
void    SysTrayXLink::slotLinkRead( QByteArray message )
{
    /*
     *  Decode the message
     */
    DecodeMessage( message );
}


/*
 *  Handle a debug state change signal
 */
void    SysTrayXLink::slotDebugChange()
{
    if( m_pref->getAppPrefChanged() )
    {
        sendPreferences();
    }
}


/*
 *  Handle the minimize type change signal
 */
void    SysTrayXLink::slotMinimizeTypeChange()
{
    if( m_pref->getAppPrefChanged() )
    {
        sendPreferences();
    }
}


/*
 *  Handle a start minimized state change signal
 */
void    SysTrayXLink::slotStartMinimizedChange()
{
    if( m_pref->getAppPrefChanged() )
    {
        sendPreferences();
    }
}


/*
 *  Handle a minimize on close state change signal
 */
void    SysTrayXLink::slotMinimizeOnCloseChange()
{
    if( m_pref->getAppPrefChanged() )
    {
        sendPreferences();
    }
}


/*
 *  Handle the default icon type change signal
 */
void    SysTrayXLink::slotDefaultIconTypeChange()
{
    if( m_pref->getAppPrefChanged() )
    {
        sendPreferences();
    }
}


/*
 *  Handle the default icon data change signal
 */
void    SysTrayXLink::slotDefaultIconDataChange()
{
    if( m_pref->getAppPrefChanged() )
    {
        sendPreferences();
    }
}


/*
 *  Handle the icon type change signal
 */
void    SysTrayXLink::slotIconTypeChange()
{
    if( m_pref->getAppPrefChanged() )
    {
        sendPreferences();
    }
}


/*
 *  Handle the icon data change signal
 */
void    SysTrayXLink::slotIconDataChange()
{
    if( m_pref->getAppPrefChanged() )
    {
        sendPreferences();
    }
}


/*
 *  Handle a show number state change signal
 */
void    SysTrayXLink::slotShowNumberChange()
{
    if( m_pref->getAppPrefChanged() )
    {
        sendPreferences();
    }
}


/*
 *  Handle a number color change signal
 */
void    SysTrayXLink::slotNumberColorChange()
{
    if( m_pref->getAppPrefChanged() )
    {
        sendPreferences();
    }
}


/*
 *  Handle the count type change signal
 */
void    SysTrayXLink::slotCountTypeChange()
{
    if( m_pref->getAppPrefChanged() )
    {
        sendPreferences();
    }
}
