#include "systrayxlink.h"

/*
 *	Local includes
 */
#include <unistd.h>

/*
 *	Qt includes
 */
#include <QFile>
#include <QDataStream>
#include <QSocketNotifier>


/*
 *	Constructor
 */
SysTrayXLink::SysTrayXLink()
{
    /*
     * Open stdin
     */
    m_stdin = new QFile( this );
    m_stdin->open( stdin, QIODevice::ReadOnly );

    /*
     * Open stdout
     */
    m_stdout = new QFile( this );
    m_stdout->open( stdout, QIODevice::WriteOnly );

    /*
     * Setup the notifiers
     */
    m_notifierLinkRead = new QSocketNotifier( STDIN_FILENO, QSocketNotifier::Read, this );
    connect( m_notifierLinkRead, &QSocketNotifier::activated, this, &SysTrayXLink::slotLinkRead );

    m_notifierLinkReadException = new QSocketNotifier( STDIN_FILENO, QSocketNotifier::Exception, this );
    connect( m_notifierLinkReadException, &QSocketNotifier::activated, this, &SysTrayXLink::slotLinkReadException );


    QDataStream out( m_stdout );

    char    reply[] = "\"Hallo other World!\"";
    qint32  replylen = sizeof( reply ) - 1;


    out.writeRawData( reinterpret_cast< char* >( &replylen ), sizeof( qint32 ) );
    out.writeRawData( reply, replylen );

}


/*
 *	Destructor
 */
SysTrayXLink::~SysTrayXLink()
{
    /*
     * Cleanup
     */
    m_stdin->close();
    delete m_stdin;

    m_stdout->close();
    delete m_stdout;

    delete m_notifierLinkRead;
    delete m_notifierLinkReadException;
}


/*
 * Read the input
 */
void    SysTrayXLink::slotLinkRead()
{
    QDataStream in( m_stdin );

    qint32  msglen;
    int status1 = in.readRawData( reinterpret_cast< char* >( &msglen ), sizeof( qint32 ) );

    emit signalReceivedMessageLength(msglen);

    QByteArray message(msglen + 1, 0 );
    int status2 = in.readRawData( message.data(), msglen );

    emit signalReceivedMessage( message );

    if( ( status1 == 4 ) && ( status2 == msglen ) )
    {
        //dummy
    }

/*
    QDataStream out( m_stdout );

    char    reply[] = "\"Hallo other World!\"";
    qint32  replylen = sizeof( reply ) - 1;

    int status3 = out.writeRawData( reinterpret_cast< char* >( &replylen ), sizeof( qint32 ) );
    int status4 = out.writeRawData( reply, replylen );
    m_stdout->flush();

    if( status3 && status4 )
    {
        //dummy
    }
*/
}


/*
 * Handle notifier exception
 */
void    SysTrayXLink::slotLinkReadException()
{
    //  Something went wrong
}




/*
 * Read the input
 */
void    SysTrayXLink::slotLinkWrite( QByteArray message )
{
    QDataStream out( m_stdout );

    qint32  msglen = message.length();
    int status1 = out.writeRawData( reinterpret_cast< char* >( &msglen ), sizeof( qint32 ) );
    int status2 = out.writeRawData( message.data(), msglen );

    m_stdout->flush();

    if( status1 && status2 )
    {
//dummy
    }
}
