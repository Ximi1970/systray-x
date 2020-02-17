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
    while( m_doWork )
    {
        qint32 data_len;
        std::cin.read( reinterpret_cast< char* >( &data_len ), sizeof( qint32 ) );

        emit signalReceivedDataLength( data_len );

        if( data_len > 0)
        {
            QByteArray data( data_len, 0 );
            std::cin.read( data.data(), data_len );

            emit signalReceivedData( data );

            /*
             *  Send the data to my parent
             */
            emit signalReceivedMessage( data );
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

    connect( reader, &SysTrayXLinkReader::signalDebugMessage, this, &SysTrayXLink::slotDebugMessage );

    connect( reader, &SysTrayXLinkReader::signalReceivedDataLength, this, &SysTrayXLink::slotReceivedDataLength );
    connect( reader, &SysTrayXLinkReader::signalReceivedData, this, &SysTrayXLink::slotReceivedData );

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
    QFile   dump("/home/maxime/dumpJSON_app2addon.txt");
    dump.open(QIODevice::WriteOnly );
    dump.write( m_pref_json_doc.toJson( QJsonDocument::Compact ).data(), m_pref_json_doc.toJson( QJsonDocument::Compact ).length() );
    dump.close();
*/


    /*
     *  Send them to the add-on
     */
    linkWrite( m_pref_json_doc.toJson( QJsonDocument::Compact ) );
}


/*
 *  Send the window normal command
 */
void    SysTrayXLink::sendWindowNormal()
{
    /*
     *  Create command
     */
    QJsonObject windowObject;
    windowObject.insert( "window", "normal" );

    /*
     *  Create doc
     */
    QJsonDocument doc( windowObject );

    /*
     *  Send the command
     */
    linkWrite( doc.toJson( QJsonDocument::Compact ) );
}


/*
 *  Send the window minimize command
 */
void    SysTrayXLink::sendWindowMinimize()
{
    /*
     *  Create command
     */
    QJsonObject windowObject;
    windowObject.insert("window", "minimized" );

    /*
     *  Create doc
     */
    QJsonDocument doc( windowObject );

    /*
     *  Send the command
     */
    linkWrite( doc.toJson( QJsonDocument::Compact ) );
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

        if( jsonObject.contains( "shutdown" ) && jsonObject[ "shutdown" ].isString() )
        {
            emit signalShutdown();
        }

        if( jsonObject.contains( "window" ) && jsonObject[ "window" ].isString() )
        {
            QString window_state = jsonObject[ "window" ].toString();
            emit signalWindowState( window_state );
        }

        if( jsonObject.contains( "preferences" ) && jsonObject[ "preferences" ].isObject() )
        {

/*
            QFile   dump("/home/maxime/dumpJSON_addon2app.txt");
            dump.open(QIODevice::WriteOnly );
            dump.write( message.data(), message.length() );
            dump.close();
*/


            DecodePreferences( jsonObject[ "preferences" ].toObject() );
        }
    }
    else
    {
        emit signalLinkReceiveError( jsonError.errorString() );
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
    prefObject.insert("iconType", QJsonValue::fromVariant( QString::number( pref.getIconType() ) ) );
    prefObject.insert("iconMime", QJsonValue::fromVariant( pref.getIconMime() ) );
    prefObject.insert("icon", QJsonValue::fromVariant( QString( pref.getIconData().toBase64() ) ) );

    QJsonObject preferencesObject;
    preferencesObject.insert("preferences", prefObject );

    /*
     *  Store the new document
     */
    m_pref_json_doc = QJsonDocument( preferencesObject );
}


/*
 *  Handle the debug message from the reader thread
 */
void    SysTrayXLink::slotDebugMessage( QString message )
{
    emit signalDebugMessage( message );
}


/*
 *  Handle data length signal from reader thread
 */
void    SysTrayXLink::slotReceivedDataLength( qint32 data_len )
{
    emit signalReceivedDataLength( data_len );
}


/*
 *  Handle data signal from read thread
 */
void    SysTrayXLink::slotReceivedData( QByteArray data )
{
    emit signalReceivedData( data );
}


/*
 *  Read the input
 */
void    SysTrayXLink::slotLinkRead( QByteArray message )
{
    /*
     *  Debug
     */
//    m_dump->write( message );

    /*
     *  Decode the message
     */
    DecodeMessage( message );
}


/*
 *  write the output
 */
void    SysTrayXLink::slotLinkWrite( QByteArray message )
{
    linkWrite( message );
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
 *  Handle the window normal signal
 */
void    SysTrayXLink::slotWindowNormal()
{
    sendWindowNormal();
}


/*
 *  Handle the window minimize signal
 */
void    SysTrayXLink::slotWindowMinimize()
{
    sendWindowMinimize();
}
