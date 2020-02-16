#include "debugwidget.h"
#include "ui_debugwidget.h"

/*
 *	Local includes
 */
#include "preferences.h"

/*
 *	System includes
 */

/*
 *	Qt includes
 */


/*
 *  Constructor
 */
DebugWidget::DebugWidget( Preferences* pref, QWidget* parent ) : QWidget( parent ), m_ui( new Ui::DebugWidget )
{
    m_ui->setupUi( this );

    /*
     *  Store the preferences
     */
    m_pref = pref;

    /*
     *  Connect the button
     */
    connect( m_ui->test1PushButton, &QPushButton::clicked, this, &DebugWidget::slotHandleTest1Button);
    connect( m_ui->test2PushButton, &QPushButton::clicked, this, &DebugWidget::slotHandleTest2Button);
}


/*
 *  Set the debug message
 */
void    DebugWidget::setDebugMessage( const QString& message )
{
    m_ui->messageLabel->setText( message );
}


/*
 *  Set the raw data length
 */
void    DebugWidget::setRawDataLength( int length )
{
    m_ui->rawDataLengthLabel->setText( QString::number( length ) );
}


/*
 *  Set the raw received message
 */
void    DebugWidget::setErrorDataMessage( const QString &message )
{
    m_ui->rawDataLabel->setText( message );
}


/*
 *  Set the number of unread mail
 */
void    DebugWidget::setUnreadMail( int unread )
{
    m_ui->unreadMailLabel->setText( QString::number( unread ) );
}


/*
 *  Set the link error message
 */
void    DebugWidget::setError( const QString &error )
{
    m_ui->errorLabel->setText( error );
}


/*
 *  Handle a debug state change signal
 */
void    DebugWidget::slotDebugChange()
{
    this->setVisible( m_pref->getDebug() );
}


/*
 *  Handle a debug message signal
 */
void    DebugWidget::slotDebugMessage( const QString& message )
{
    setDebugMessage( message );
}


/*
 *  Handle received message length
 */
void    DebugWidget::slotReceivedDataLength( qint32 data_len )
{
    setRawDataLength( data_len );
}


/*
 *  Display received message
 */
void    DebugWidget::slotReceivedData( const QByteArray& data )
{
    setErrorDataMessage( QString( data ) );

    /*
     * Reply
     */
//    QByteArray reply = QString( "\"Hallo other world!\"" ).toUtf8();
//    emit signalWriteMessage( reply );
}


/*
 *  Handle unread mail signal
 */
void    DebugWidget::slotUnreadMail( int unread_mail )
{
    setUnreadMail( unread_mail );
}


/*
 *  Handle a receive error
 */
void    DebugWidget::slotReceiveError( const QString& error )
{
    setError( error );
}


/*
 *  Handle test button 1 click
 */
void    DebugWidget::slotHandleTest1Button()
{
    emit signalTest1ButtonClicked();
}


/*
 *  Handle test button 2 click
 */
void    DebugWidget::slotHandleTest2Button()
{
    emit signalTest2ButtonClicked();
}


/*
 *  Handle console signal
 */
void    DebugWidget::slotConsole( QString message )
{
    m_ui->textEdit->append( message );
}
