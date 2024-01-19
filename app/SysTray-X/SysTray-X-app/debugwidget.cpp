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
    connect( m_ui->test4PushButton, &QPushButton::clicked, this, &DebugWidget::slotHandleTest4Button);
}


/*
 *  Handle the language change event
 */
void    DebugWidget::changeEvent( QEvent* event )
{
    QWidget::changeEvent( event );

    if( event->type() == QEvent::LanguageChange )
    {
        m_ui->retranslateUi( this );
    }
}


/*
 *  Set the number of unread/new mails
 */
void    DebugWidget::setMailCount( int unread_mail, int new_mail )
{
    m_ui->unreadMailLabel->setText( QString::number( unread_mail ) );
    m_ui->newMailLabel->setText( QString::number( new_mail ) );
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
void    DebugWidget::slotMailCount( int unread_mail, int new_mail )
{
    setMailCount( unread_mail, new_mail );
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
 *  Handle test button 4 click
 */
void    DebugWidget::slotHandleTest4Button()
{
    emit signalTest4ButtonClicked();
}


/*
 *  Handle console signal
 */
void    DebugWidget::slotConsole( QString message )
{
    m_ui->textEdit->append( message );
}
