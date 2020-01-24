#include "systrayxlink.h"

/*
 *	Local includes
 */
#include "preferences.h"

/*
 *  System includes
 */
#include <unistd.h>

/*
 *	Qt includes
 */
#include <QFile>
#include <QString>
#include <QVariant>
#include <QDataStream>
#include <QJsonObject>
#include <QJsonValue>
#include <QSocketNotifier>

/*
 *	Constructor
 */
SysTrayXLink::SysTrayXLink( Preferences *pref )
{
    /*
     *  Store preferences
     */
    m_pref = pref;

    /*
     *  Open stdin
     */
    m_stdin = new QFile( this );
    m_stdin->open( stdin, QIODevice::ReadOnly );

    /*
     *  Open stdout
     */
    m_stdout = new QFile( this );
    m_stdout->open( stdout, QIODevice::WriteOnly );

    /*
     *  Open dump.txt
     */
    m_dump = new QFile( "dump.txt", this );
    m_dump->open( QIODevice::WriteOnly );

    /*
     *  Setup the notifiers
     */
    m_notifier_link_read = new QSocketNotifier( STDIN_FILENO, QSocketNotifier::Read, this );
    connect( m_notifier_link_read, &QSocketNotifier::activated, this, &SysTrayXLink::slotLinkRead );

    m_notifier_link_read_exception = new QSocketNotifier( STDIN_FILENO, QSocketNotifier::Exception, this );
    connect( m_notifier_link_read_exception, &QSocketNotifier::activated, this, &SysTrayXLink::slotLinkReadException );
}


/*
 *	Destructor
 */
SysTrayXLink::~SysTrayXLink()
{
    /*
     *  Cleanup
     */
    m_stdin->close();
    delete m_stdin;

    m_stdout->close();
    delete m_stdout;

    m_dump->close();
    delete m_dump;

    delete m_notifier_link_read;
    delete m_notifier_link_read_exception;
}


/*
 *  Write a message to the link
 */
void SysTrayXLink::linkWrite( const QByteArray& message )
{
    QDataStream out( m_stdout );

    qint32  msglen = message.length();
    int status1 = out.writeRawData( reinterpret_cast< char* >( &msglen ), sizeof( qint32 ) );
    int status2 = out.writeRawData( message.data(), msglen );

    m_stdout->flush();


    if( status1 && status2 )
    {
        //error handling?
    }
}


/*
 *  Send the preferences to the add-on
 */
void SysTrayXLink::sendPreferences()
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
 *  Decode JSON message
 */
void SysTrayXLink::DecodeMessage( const QByteArray& message )
{
    QJsonParseError jsonError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson( message, &jsonError );

    if( jsonError.error == QJsonParseError::NoError )
    {
        QJsonObject jsonObject = jsonResponse.object();

        if( jsonObject.contains( "shutdown" ) && jsonObject[ "shutdown" ].isString() )
        {
            emit signalShutdown();
        }

        if( jsonObject.contains( "unreadMail" ) && jsonObject[ "unreadMail" ].isDouble() )
        {
            int unreadMail = jsonObject[ "unreadMail" ].toInt();
            emit signalUnreadMail( unreadMail );
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
void SysTrayXLink::DecodePreferences( const QJsonObject& pref )
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
void SysTrayXLink::EncodePreferences( const Preferences& pref )
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
 *  Read the input
 */
void SysTrayXLink::slotLinkRead()
{
    QDataStream in( m_stdin );

    qint32  msglen;
    int status1 = in.readRawData( reinterpret_cast< char* >( &msglen ), sizeof( qint32 ) );

    emit signalReceivedMessageLength( msglen );

    QByteArray message(msglen, 0 );
    int status2 = in.readRawData( message.data(), msglen );

    emit signalReceivedMessage( message );

    m_dump->write( message );

    /*
     *  Decode the message
     */
    DecodeMessage( message );


    if( ( status1 == 4 ) && ( status2 == msglen ) )
    {
        //error handling?
    }
}


/*
 *  Handle read notifier exception
 */
void SysTrayXLink::slotLinkReadException()
{
    //  Something went wrong
}


/*
 *  write the output
 */
void SysTrayXLink::slotLinkWrite( QByteArray message )
{
    linkWrite( message );
}



/*
 *  Handle a debug state change signal
 */
void SysTrayXLink::slotDebugChange()
{
    if( m_pref->getAppPrefChanged() )
    {
        sendPreferences();
    }
}

/*
 *  Handle the icon type change signal
 */
void SysTrayXLink::slotIconTypeChange()
{
    if( m_pref->getAppPrefChanged() )
    {
        sendPreferences();
    }
}


/*
 *  Handle the icon data change signal
 */
void SysTrayXLink::slotIconDataChange()
{
    if( m_pref->getAppPrefChanged() )
    {
        sendPreferences();
    }
}
