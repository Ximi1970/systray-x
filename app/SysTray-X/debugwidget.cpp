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
    connect( m_ui->test3PushButton, &QPushButton::clicked, this, &DebugWidget::slotHandleTest3Button);
}


/*
 *  Handle the language change event
 */
void    DebugWidget::changeEvent( QEvent* event )
{
    if( event->type() == QEvent::LanguageChange )
    {
        m_ui->retranslateUi( this );
    }
}


/*
 *  Set the number of unread mail
 */
void    DebugWidget::setUnreadMail( int unread )
{
    m_ui->unreadMailLabel->setText( QString::number( unread ) );
}


/*
 *  Handle a debug state change signal
 */
void    DebugWidget::slotDebugChange()
{
    this->setVisible( m_pref->getDebug() );
}


/*
 *  Handle unread mail signal
 */
void    DebugWidget::slotUnreadMail( int unread_mail )
{
    setUnreadMail( unread_mail );
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
 *  Handle test button 3 click
 */
void    DebugWidget::slotHandleTest3Button()
{
    emit signalTest3ButtonClicked();
}


/*
 *  Handle console signal
 */
void    DebugWidget::slotConsole( QString message )
{
    m_ui->textEdit->append( message );
}
