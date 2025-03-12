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
#include <QJsonArray>


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
    m_do_work = true;

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
    m_do_work = false;
}


/*
 *	Read the data
 */
void    SysTrayXLinkReader::slotWorker()
{
    int error_count = 0;

    while( m_do_work )
    {
        qint32 data_len;
        std::cin.read( reinterpret_cast< char* >( &data_len ), sizeof( qint32 ) );

        if( data_len > 0)
        {
            QByteArray data( data_len, 0 );
            std::cin.read( data.data(), data_len );

            if( data.at( 0 ) == '{' )
            {                
                /*
                 *  Send the data to my parent
                 */
                emit signalReceivedMessage( data );

                error_count = 0;
            }
            else
            {
                error_count++;

                if( error_count > 20 )
                {
                    emit signalErrorAddOnShutdown();
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
    connect( reader, &SysTrayXLinkReader::signalErrorAddOnShutdown, this, &SysTrayXLink::slotErrorAddOnShutdown );

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
 *  Send the options to the add-on
 */
void    SysTrayXLink::sendOptions()
{
    QJsonObject options_object;
    options_object.insert("kdeIntegration", QJsonValue::fromVariant( m_pref->getKdeIntegrationOption() ) );
    options_object.insert("shortcuts", QJsonValue::fromVariant( m_pref->getShortcutsOption() ) );

    QJsonObject main_object;
    main_object.insert("options", options_object);

    /*
     *  Store the new document
     */
    QJsonDocument json_doc = QJsonDocument( main_object );

    /*
     *  Send it to the add-on
     */
    linkWrite( json_doc.toJson( QJsonDocument::Compact ) );
}


/*
 *  Send the window positions to the add-on
 */
void    SysTrayXLink::sendPositions( QList< QPoint > positions )
{
    QJsonArray positionsArray;

    for( int i = 0; i < positions.length(); ++i )
    {
        QJsonObject positionObject;
        positionObject.insert("x", QString::number( positions.at( i ).x() ) );
        positionObject.insert("y", QString::number( positions.at( i ).y() ) );

        positionsArray.append( positionObject );
    }

    QJsonObject positionsObject;
    positionsObject.insert("positions", positionsArray );

    /*
     *  Store the new document
     */
    QJsonDocument json_doc = QJsonDocument( positionsObject );

    /*
     *  Send it to the add-on
     */
    linkWrite( json_doc.toJson( QJsonDocument::Compact ) );
}


/*
 *  Send a new message request.
*/
void    SysTrayXLink::sendNewMessage( const QString& from )
{
    QJsonObject newMessageObject;
    newMessageObject.insert("newMessage", QJsonValue::fromVariant( from ) );

    /*
     *  Store the new document
     */
    QJsonDocument json_doc = QJsonDocument( newMessageObject );

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

/*
        QStringList list = jsonObject.keys();
        for( int i = 0 ;  i < list.length() ; ++i )
        {
            emit signalConsole( QString("Message %1").arg(list.at(i)) );
        }
*/

        if( jsonObject.contains( "mailCount" ) && jsonObject[ "mailCount" ].isObject() )
        {
            QJsonObject mailCount = jsonObject[ "mailCount" ].toObject();

            /*
             *  Check the received object
             */
            int unreadMail = 0;
            int newMail = 0;
            if( mailCount.contains( "unread" ) && mailCount[ "unread" ].isDouble() )
            {
                unreadMail = mailCount[ "unread" ].toInt();
            }

            if( mailCount.contains( "new" ) && mailCount[ "new" ].isDouble() )
            {
                newMail = mailCount[ "new" ].toInt();
            }

            emit signalMailCount( unreadMail, newMail );
        }

        if( jsonObject.contains( "version" ) && jsonObject[ "version" ].isString() )
        {
            QString version = jsonObject[ "version" ].toString();
            emit signalVersion( version );
        }

        if( jsonObject.contains( "shutdown" ) && jsonObject[ "shutdown" ].isString() )
        {
            /*
             *  Launch close appplication
             */
            emit signalCloseApp();

            /*
             *  Shutdown the addon
             */
            emit signalAddOnShutdown();
        }

        if( jsonObject.contains( "startup" ) && jsonObject[ "startup" ].isString() )
        {
            QString state_str = jsonObject[ "startup" ].toString();

            Preferences::WindowState window_state;
            if( state_str == Preferences::STATE_MINIMIZED_STR )
            {
                window_state = Preferences::STATE_MINIMIZED_STARTUP;
            }

            if( state_str == Preferences::STATE_DOCKED_STR )
            {
                window_state = Preferences::STATE_DOCKED_STARTUP;
            }

            emit signalWindowState( window_state, 0 );
        }

        if( jsonObject.contains( "windows" ) && jsonObject[ "windows" ].isArray() )
        {
            QJsonArray windows = jsonObject[ "windows" ].toArray();

            for( int i = 0 ; i < windows.count() ; ++i )
            {
                QJsonObject window = windows[ i ].toObject();

                int window_id = 0;
                if( window.contains( "id" ) && window[ "id" ].isDouble() )
                {
                    window_id = window[ "id" ].toInt();
                }

                QString window_state_str;
                if( window.contains( "state" ) && window[ "state" ].isString() )
                {
                    window_state_str = window[ "state" ].toString();
                }

                Preferences::WindowState window_state;
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
                {
                    /*
                     *  Unknown state
                     */
                    window_state = Preferences::STATE_NORMAL;
                }

                if( window_id != 0 )
                {
                    emit signalWindowState( window_state, window_id );
                }
            }
        }

        if( jsonObject.contains( "optionsRequest" ) && jsonObject[ "optionsRequest" ].isBool() )
        {
            sendOptions();
        }

        if( jsonObject.contains( "hideDefaultIcon" ) && jsonObject[ "hideDefaultIcon" ].isBool() )
        {
            bool hide_default_icon = jsonObject[ "hideDefaultIcon" ].toBool();

            /*
             *  Signal the KDE integration or hide default icon
             */
            emit signalKdeIntegration( hide_default_icon );
        }

        if( jsonObject.contains( "locale" ) && jsonObject[ "locale" ].isString() )
        {
            QString locale = jsonObject[ "locale" ].toString();

            emit signalLocale( locale );
        }

        if( jsonObject.contains( "platformInfo" ) && jsonObject[ "platformInfo" ].isObject() )
        {
            DecodePlatform( jsonObject[ "platformInfo" ].toObject() );
        }

        if( jsonObject.contains( "browserInfo" ) && jsonObject[ "browserInfo" ].isObject() )
        {
            DecodeBrowser( jsonObject[ "browserInfo" ].toObject() );
        }

        if( jsonObject.contains( "positions" ) && jsonObject[ "positions" ].isArray() )
        {
            DecodePositions( jsonObject[ "positions" ].toArray() );
        }

        if( jsonObject.contains( "preferences" ) && jsonObject[ "preferences" ].isObject() )
        {
            DecodePreferences( jsonObject[ "preferences" ].toObject() );
        }

        if( jsonObject.contains( "startApp" ) && jsonObject[ "startApp" ].isString() &&
            jsonObject.contains( "startAppArgs" ) && jsonObject[ "startAppArgs" ].isString() )
        {
            QString app = jsonObject[ "startApp" ].toString();
            QString args = jsonObject[ "startAppArgs" ].toString();

            /*
             *  Store the new start application parameters
             */
            m_pref->setStartApp( app );
            m_pref->setStartAppArgs( args );

            emit signalStartApp();
        }

        if( jsonObject.contains( "closeApp" ) && jsonObject[ "closeApp" ].isString() )
        {
            emit signalCloseApp();
        }

        if( jsonObject.contains( "newWindow" ) && jsonObject[ "newWindow" ].isDouble() )
        {
            int new_window_id = jsonObject[ "newWindow" ].toInt();

            emit signalNewWindow( new_window_id );
        }

        if( jsonObject.contains( "closeWindow" ) && jsonObject[ "closeWindow" ].isObject() )
        {
            QJsonObject close_window = jsonObject[ "closeWindow" ].toObject();

            int close_window_id = 0;
            if( close_window.contains( "id" ) && close_window[ "id" ].isDouble() )
            {
                close_window_id = close_window[ "id" ].toInt();
            }

            bool close_quit = false;
            if( close_window.contains( "quit" ) && close_window[ "quit" ].isBool() )
            {
                close_quit = close_window[ "quit" ].toBool();
            }

            emit signalCloseWindow( close_window_id, close_quit );
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
 *  Decode positions from JSON message
 */
void    SysTrayXLink::DecodePositions( const QJsonArray& positions )
{
    QList< QPoint > window_positions;
    for( int i = 0; i < positions.count(); ++i )
    {
        QJsonObject positionObject = positions.at( i ).toObject();

        QPoint  position;
        bool valid_x = false;
        bool valid_y = false;
        if( positionObject.contains( "x" ) && positionObject[ "x" ].isString() )
        {
            position.setX( positionObject[ "x" ].toString().toInt() );
            valid_x = true;
        }

        if( positionObject.contains( "y" ) && positionObject[ "y" ].isString() )
        {
            position.setY( positionObject[ "y" ].toString().toInt() );
            valid_y = true;
        }

        if( valid_x && valid_y )
        {
            window_positions.append( position );
        }
    }

    emit signalWindowPositions( window_positions );
}


/*
 *  Decode preferences from JSON message
 */
void    SysTrayXLink::DecodePreferences( const QJsonObject& pref )
{
    if( pref.contains( "debug" ) && pref[ "debug" ].isString() )
    {
        bool debug = pref[ "debug" ].toString() == "true";

        /*
         *  Store the new debug state
         */
        m_pref->setDebug( debug );
    }

    if( pref.contains( "minimizeType" ) && pref[ "minimizeType" ].isString() )
    {
        Preferences::MinimizeType minimize_type = static_cast< Preferences::MinimizeType >( pref[ "minimizeType" ].toString().toInt() );

        /*
         *  Store the new minimize type
         */
        m_pref->setMinimizeType( minimize_type );
    }

    if( pref.contains( "minimizeIconType" ) && pref[ "minimizeIconType" ].isString() )
    {
        Preferences::MinimizeIconType minimize_icon_type = static_cast< Preferences::MinimizeIconType >( pref[ "minimizeIconType" ].toString().toInt() );

        /*
         *  Store the new minimize type
         */
        m_pref->setMinimizeIconType( minimize_icon_type );
    }

    if( pref.contains( "closeType" ) && pref[ "closeType" ].isString() )
    {
        Preferences::CloseType close_type = static_cast< Preferences::CloseType >( pref[ "closeType" ].toString().toInt() );

        /*
         *  Store the new close type
         */
        m_pref->setCloseType( close_type );
    }

    if( pref.contains( "startupType" ) && pref[ "startupType" ].isString() )
    {
        Preferences::StartupType startup_type = static_cast< Preferences::StartupType >( pref[ "startupType" ].toString().toInt() );

        /*
         *  Store the new start minimized state
         */
        m_pref->setStartupType( startup_type );
    }

    if( pref.contains( "windowPosCor" ) && pref[ "windowPosCor" ].isString() )
    {
        bool window_positions_correction = pref[ "windowPosCor" ].toString() == "true";

        /*
         *  Store the new window positions correction
         */
        m_pref->setWindowPositionsCorrection( window_positions_correction );
    }

    if( pref.contains( "windowPosCorType" ) && pref[ "windowPosCorType" ].isString() )
    {
        Preferences::WindowPositionsCorrectionType window_positions_correction_type = static_cast< Preferences::WindowPositionsCorrectionType >( pref[ "windowPosCorType" ].toString().toInt() );

        /*
         *  Store the window positions correction type
         */
        m_pref->setWindowPositionsCorrectionType( window_positions_correction_type );
    }

    if( pref.contains( "restorePositions" ) && pref[ "restorePositions" ].isString() )
    {
        bool resore_window_positions = pref[ "restorePositions" ].toString() == "true";

        /*
         *  Store the new restore window positions state
         */
        m_pref->setRestoreWindowPositions( resore_window_positions );
    }

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

    if( pref.contains( "hideDefaultIcon" ) && pref[ "hideDefaultIcon" ].isString() )
    {
        bool hide_default_icon = pref[ "hideDefaultIcon" ].toString() == "true";

        /*
         *  Store the new hide default icon state
         */
        m_pref->setHideDefaultIcon( hide_default_icon );
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

    if( pref.contains( "invertIcon" ) && pref[ "invertIcon" ].isString() )
    {
        bool invert_icon = pref[ "invertIcon" ].toString() == "true";

        /*
         *  Store the new invert icon state
         */
        m_pref->setInvertIcon( invert_icon );
    }

    if( pref.contains( "showNumber" ) && pref[ "showNumber" ].isString() )
    {
        bool show_number = pref[ "showNumber" ].toString() == "true";

        /*
         *  Store the new show number state
         */
        m_pref->setShowNumber( show_number );
    }

    if( pref.contains( "showNewIndicator" ) && pref[ "showNewIndicator" ].isString() )
    {
        bool show_new_indicator = pref[ "showNewIndicator" ].toString() == "true";

        /*
         *  Store the new show new indicator state
         */
        m_pref->setShowNewIndicator( show_new_indicator );
    }

    if( pref.contains( "countType" ) && pref[ "countType" ].isString() )
    {
        Preferences::CountType count_type = static_cast< Preferences::CountType >( pref[ "countType" ].toString().toInt() );

        /*
         *  Store the new count type
         */
        m_pref->setCountType( count_type );
    }

    if( pref.contains( "startupDelay" ) && pref[ "startupDelay" ].isString() )
    {
        int startup_delay = pref[ "startupDelay" ].toString().toInt();

        /*
         *  Store the new startup delay
         */
        m_pref->setStartupDelay( startup_delay );
    }

    if( pref.contains( "apiCountMethod" ) && pref[ "apiCountMethod" ].isString() )
    {
        bool api_count_method = pref[ "apiCountMethod" ].toString() == "true";

        /*
         *  Store the new API cont method state
         */
        m_pref->setApiCountMethod( api_count_method );
    }

    if( pref.contains( "numberColor" ) && pref[ "numberColor" ].isString() )
    {
        QString number_color = pref[ "numberColor" ].toString();

        /*
         *  Store the new number color
         */
        m_pref->setNumberColor( number_color );
    }

    if( pref.contains( "numberSize" ) && pref[ "numberSize" ].isString() )
    {
        int number_size = pref[ "numberSize" ].toString().toInt();

        /*
         *  Store the new number size
         */
        m_pref->setNumberSize( number_size );
    }

    if( pref.contains( "numberAlignment" ) && pref[ "numberAlignment" ].isString() )
    {
        int number_alignment = pref[ "numberAlignment" ].toString().toInt();

        /*
         *  Store the new number alignment
         */
        m_pref->setNumberAlignment( number_alignment );
    }

    if( pref.contains( "numberMargins" ) && pref[ "numberMargins" ].isObject() )
    {
        QMargins margins = DecodeMargins( pref[ "numberMargins" ].toObject() );

        /*
         *  Store the new number size
         */
        m_pref->setNumberMargins( margins );
    }

    if( pref.contains( "newIndicatorType" ) && pref[ "newIndicatorType" ].isString() )
    {
        Preferences::NewIndicatorType new_indicator_type = static_cast< Preferences::NewIndicatorType >( pref[ "newIndicatorType" ].toString().toInt() );

        /*
         *  Store the new indicator type
         */
        m_pref->setNewIndicatorType( new_indicator_type );
    }

    if( pref.contains( "newShadeColor" ) && pref[ "newShadeColor" ].isString() )
    {
        QString new_shade_color = pref[ "newShadeColor" ].toString();

        /*
         *  Store the new shade color
         */
        m_pref->setNewShadeColor( new_shade_color );
    }

    if( pref.contains( "startApp" ) && pref[ "startApp" ].isString() )
    {
        QString app = pref[ "startApp" ].toString();

        /*
         *  Store the new start application
         */
        m_pref->setStartApp( app );
    }

    if( pref.contains( "startAppArgs" ) && pref[ "startAppArgs" ].isString() )
    {
        QString args = pref[ "startAppArgs" ].toString();

        /*
         *  Store the new start application arguments
         */
        m_pref->setStartAppArgs( args );
    }

    if( pref.contains( "closeApp" ) && pref[ "closeApp" ].isString() )
    {
        QString app = pref[ "closeApp" ].toString();

        /*
         *  Store the new close application
         */
        m_pref->setCloseApp( app );
    }

    if( pref.contains( "closeAppArgs" ) && pref[ "closeAppArgs" ].isString() )
    {
        QString args = pref[ "closeAppArgs" ].toString();

        /*
         *  Store the new close application arguments
         */
        m_pref->setCloseAppArgs( args );
    }

    if( pref.contains( "showHideShortcut" ) && pref[ "showHideShortcut" ].isString() )
    {
        QString shortcut = pref[ "showHideShortcut" ].toString();

        /*
         *  Store the new shortcut
         */
        m_pref->setShowHideShortcut( QKeySequence::fromString( shortcut ) );
    }

    if( pref.contains( "newMessageFroms" ) && pref[ "newMessageFroms" ].isArray() )
    {
        QJsonArray froms = pref[ "newMessageFroms" ].toArray();

        QStringList fromsList;
        for( int i = 0 ; i < froms.size() ; ++i )
        {
            fromsList.append( froms.at( i ).toString());
        }

        /*
         *  Store the new message from list
         */
        m_pref->setNewMessageFroms( fromsList );
    }
}


/*
 *  Decode preferences from JSON message
 */
QMargins    SysTrayXLink::DecodeMargins( const QJsonObject& margins_json )
{
    QMargins    margins;
    if( margins_json.contains( "left" ) && margins_json[ "left" ].isString() )
    {
        margins.setLeft( margins_json[ "left" ].toString().toInt() );
    }

    if( margins_json.contains( "top" ) && margins_json[ "top" ].isString() )
    {
        margins.setTop( margins_json[ "top" ].toString().toInt() );
    }

    if( margins_json.contains( "right" ) && margins_json[ "right" ].isString() )
    {
        margins.setRight( margins_json[ "right" ].toString().toInt() );
    }

    if( margins_json.contains( "bottom" ) && margins_json[ "bottom" ].isString() )
    {
        margins.setBottom( margins_json[ "bottom" ].toString().toInt() );
    }

    return margins;
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
    prefObject.insert( "debug", QJsonValue::fromVariant( QString( pref.getDebug() ? "true" : "false" ) ) );
    prefObject.insert( "minimizeType", QJsonValue::fromVariant( QString::number( pref.getMinimizeType() ) ) );
    prefObject.insert( "minimizeIconType", QJsonValue::fromVariant( QString::number( pref.getMinimizeIconType() ) ) );
    prefObject.insert( "startupType", QJsonValue::fromVariant( QString::number( pref.getStartupType() ) ) );
    prefObject.insert( "windowPosCor", QJsonValue::fromVariant( QString( pref.getWindowPositionsCorrection() ? "true" : "false" ) ) );
    prefObject.insert( "windowPosCorType", QJsonValue::fromVariant( QString::number( pref.getWindowPositionsCorrectionType() ) ) );
    prefObject.insert( "restorePositions", QJsonValue::fromVariant( QString( pref.getRestoreWindowPositions() ? "true" : "false" ) ) );
    prefObject.insert( "closeType", QJsonValue::fromVariant( QString::number( pref.getCloseType() ) ) );
    prefObject.insert( "defaultIconType", QJsonValue::fromVariant( QString::number( pref.getDefaultIconType() ) ) );
    prefObject.insert( "defaultIconMime", QJsonValue::fromVariant( pref.getDefaultIconMime() ) );
    prefObject.insert( "defaultIcon", QJsonValue::fromVariant( QString( pref.getDefaultIconData().toBase64() ) ) );
    prefObject.insert( "hideDefaultIcon", QJsonValue::fromVariant( QString( pref.getHideDefaultIcon() ? "true" : "false" ) ) );
    prefObject.insert( "iconType", QJsonValue::fromVariant( QString::number( pref.getIconType() ) ) );
    prefObject.insert( "iconMime", QJsonValue::fromVariant( pref.getIconMime() ) );
    prefObject.insert( "icon", QJsonValue::fromVariant( QString( pref.getIconData().toBase64() ) ) );
    prefObject.insert( "invertIcon", QJsonValue::fromVariant( QString( pref.getInvertIcon() ? "true" : "false" ) ) );
    prefObject.insert( "showNumber", QJsonValue::fromVariant( QString( pref.getShowNumber() ? "true" : "false" ) ) );
    prefObject.insert( "showNewIndicator", QJsonValue::fromVariant( QString( pref.getShowNewIndicator() ? "true" : "false" ) ) );
    prefObject.insert( "numberColor", QJsonValue::fromVariant( QString( pref.getNumberColor() ) ) );
    prefObject.insert( "numberSize", QJsonValue::fromVariant( QString::number( pref.getNumberSize() ) ) );
    prefObject.insert( "numberAlignment", QJsonValue::fromVariant( QString::number( pref.getNumberAlignment() ) ) );

    QJsonObject marginsObject;
    marginsObject.insert( "left", QJsonValue::fromVariant( QString::number( pref.getNumberMargins().left() ) ) );
    marginsObject.insert( "top", QJsonValue::fromVariant( QString::number( pref.getNumberMargins().top() ) ) );
    marginsObject.insert( "right", QJsonValue::fromVariant( QString::number( pref.getNumberMargins().right() ) ) );
    marginsObject.insert( "bottom", QJsonValue::fromVariant( QString::number( pref.getNumberMargins().bottom() ) ) );

    prefObject.insert( "numberMargins", marginsObject );
    prefObject.insert( "countType", QJsonValue::fromVariant( QString::number( pref.getCountType() ) ) );
    prefObject.insert( "startupDelay", QJsonValue::fromVariant( QString::number( pref.getStartupDelay() ) ) );
    prefObject.insert( "apiCountMethod", QJsonValue::fromVariant( QString( pref.getApiCountMethod() ? "true" : "false" ) ) );

    prefObject.insert( "newIndicatorType", QJsonValue::fromVariant( QString::number( pref.getNewIndicatorType() ) ) );
    prefObject.insert( "newShadeColor", QJsonValue::fromVariant( QString( pref.getNewShadeColor() ) ) );

    prefObject.insert( "startApp", QJsonValue::fromVariant( pref.getStartApp() ) );
    prefObject.insert( "startAppArgs", QJsonValue::fromVariant( pref.getStartAppArgs() ) );
    prefObject.insert( "closeApp", QJsonValue::fromVariant( pref.getCloseApp() ) );
    prefObject.insert( "closeAppArgs", QJsonValue::fromVariant( pref.getCloseAppArgs() ) );

    prefObject.insert( "showHideShortcut", QJsonValue::fromVariant( pref.getShowHideShortcut().toString() ) );

    QJsonObject preferencesObject;
    preferencesObject.insert( "preferences", prefObject );

    /*
     *  Store the new document
     */
    m_pref_json_doc = QJsonDocument( preferencesObject );
}


/*
 *  Relay shutdown signal
 */
void    SysTrayXLink::slotErrorAddOnShutdown()
{
    emit signalErrorAddOnShutdown();
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
 *  Handle a positions change signal
 */
void    SysTrayXLink::slotPositions( QList< QPoint > positions )
{
    sendPositions( positions );
}


/*
 *  Handle a preferences changed signal
 */
void    SysTrayXLink::slotPreferencesChanged()
{
    sendPreferences();
}
