#include "systrayxicon.h"

/*
 *	Local includes
 */
#include "preferences.h"

/*
 *  System includes
 */
#include "systrayxlink.h"

/*
 *	Qt includes
 */
#include <QPainter>


/*
 *	Constructor
 */
SysTrayXIcon::SysTrayXIcon( SysTrayXLink *link, Preferences *pref, QObject *parent ) : QSystemTrayIcon( QIcon(), parent )
{
    /*
     *  Initialize
     */
    m_link = link;
    m_pref = pref;

    m_unread_mail = 0;
}


/*
 *  Set the icon type
 */
void SysTrayXIcon::setIconType( Preferences::IconType icon_type )
{
    if( icon_type != m_icon_type )
    {
        /*
         *  Store the new value
         */
        m_icon_type = icon_type;

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}


/*
 *  Set the icon mime
 */
void SysTrayXIcon::setIconMime( const QString& icon_mime )
{
    if( m_icon_mime != icon_mime )
    {
        /*
         *  Store the new value
         */
        m_icon_mime = icon_mime;
    }
}


/*
 *  Set the icon type
 */
void SysTrayXIcon::setIconData( const QByteArray& icon_data )
{
    if( m_icon_data != icon_data )
    {
        /*
         *  Store the new value
         */
        m_icon_data = icon_data;

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}


/*
 *  Set the number of unread mails
 */
void SysTrayXIcon::setUnreadMail( int unread_mail )
{
    if( unread_mail != m_unread_mail ) {

        /*
         *  Store the new value
         */
        m_unread_mail = unread_mail;

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}


/*
 *  Set and render the icon in the system tray
 */
void SysTrayXIcon::renderIcon()
{
    QPixmap pixmap;

    switch( m_icon_type )
    {
        case Preferences::PREF_BLANK_ICON:
        {
            pixmap = QPixmap( ":/files/icons/blank-icon.png" );
            break;
        }

        case Preferences::PREF_NEWMAIL_ICON:
        {
            QIcon new_mail = QIcon::fromTheme("mail-unread", QIcon(":/files/icons/blank-icon.png"));
            pixmap = new_mail.pixmap( 256, 256 );
            break;
        }

        case Preferences::PREF_CUSTOM_ICON:
        {
            pixmap.loadFromData( m_icon_data );
            break;
        }
    }

    QString number = QString::number( m_unread_mail );

    if( m_unread_mail > 0 )
    {
        /*
         *  Paint the number
         */
        QPainter painter( &pixmap );

        painter.setFont( QFont("Sans") );

        double factor = pixmap.width() / ( 3 * painter.fontMetrics().width( number ) );
        QFont font = painter.font();
        font.setPointSizeF( font.pointSizeF() * factor );
        font.setBold( true );
        painter.setFont( font );

        painter.drawText( pixmap.rect(), Qt::AlignCenter, QString::number( m_unread_mail ) );
    }

    /*
     *  Set the tray icon
     */
    QSystemTrayIcon::setIcon( QIcon( pixmap ) );
}


/*
 *  Handle unread mail signal
 */
void SysTrayXIcon::slotSetUnreadMail( int unread_mail )
{
    setUnreadMail( unread_mail );
}


/*
 *  Handle the icon type change signal
 */
void SysTrayXIcon::slotIconTypeChange()
{
    setIconType( m_pref->getIconType() );
}


/*
 *  Handle the icon data change signal
 */
void SysTrayXIcon::slotIconDataChange()
{
    setIconMime( m_pref->getIconMime() );
    setIconData( m_pref->getIconData() );
}
