#include "debugwidget.h"
#include "ui_debugwidget.h"

/*
 *	Local includes
 */
#include "preferences.h"

/*
 *	Qt includes
 */


/*
 *  Constructor
 */
DebugWidget::DebugWidget( Preferences *pref, QWidget *parent ) : QWidget( parent ), m_ui( new Ui::DebugWidget )
{
    m_ui->setupUi( this );

    /*
     *  Store the preferences
     */
    m_pref = pref;
}


/*
 *  Set the debug message
 */
void DebugWidget::setDebugMessage( QString message )
{
    m_ui->messageLabel->setText( message );
}


/*
 *  Set the raw data length
 */
void DebugWidget::setRawDataLength( int length )
{
    m_ui->rawDataLengthLabel->setText( QString::number( length ) );
}


/*
 *  Set the raw received message
 */
void DebugWidget::setErrorDataMessage( const QString &message )
{
    m_ui->rawDataLabel->setText( message );
}


/*
 *  Set the number of unread mail
 */
void DebugWidget::setUnreadMail( int unread )
{
    m_ui->unreadMailLabel->setText( QString::number( unread ) );
}


/*
 *  Set the link error message
 */
void DebugWidget::setError( const QString &error )
{
    m_ui->errorLabel->setText( error );
}


/*
 *  Handle a debug state change signal
 */
void DebugWidget::slotDebugChange()
{
    this->setVisible( m_pref->getDebug() );
}


/*
 *  Handle a debug message signal
 */
void DebugWidget::slotDebugMessage( QString message )
{
    setDebugMessage( message );
}


/*
 *  Handle received message length
 */
void DebugWidget::slotReceivedMessageLength( qint32 msglen )
{
    setRawDataLength( msglen );
}


/*
 *  Display received message
 */
void DebugWidget::slotReceivedMessage( QByteArray message )
{
    setErrorDataMessage( QString( message ) );

    /*
     * Reply
     */
//    QByteArray reply = QString( "\"Hallo other world!\"" ).toUtf8();
//    emit signalWriteMessage( reply );
}


/*
 *  Handle unread mail signal
 */
void DebugWidget::slotUnreadMail( int unread_mail )
{
    setUnreadMail( unread_mail );
}


/*
 *  Handle a receive error
 */
void DebugWidget::slotReceiveError( QString error )
{
    setError( error );
}
