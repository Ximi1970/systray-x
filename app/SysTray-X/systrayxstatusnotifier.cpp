#include "systrayxstatusnotifier.h"

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
#include <QIcon>
#include <QPixmap>
#include <QPainter>


/*
 *	Constructor
 */
SysTrayXStatusNotifier::SysTrayXStatusNotifier( SysTrayXLink* link, Preferences* pref, QObject* parent )
    : KStatusNotifierItem( parent )
{
    /*
     *  Initialize
     */
    m_link = link;
    m_pref = pref;

    m_unread_mail = 0;

    m_show_number = m_pref->getShowNumber();
    m_number_color = m_pref->getNumberColor();
    m_number_size = m_pref->getNumberSize();

    /*
     * Setup notifier
     */
    setCategory( KStatusNotifierItem::ApplicationStatus );

    setIconByPixmap( QIcon( QPixmap( ":/files/icons/Thunderbird.png") ) );
    setTitle("SysTray-X");

//  setStatus(KStatusNotifierItem::ItemStatus::Passive);
    setStatus(KStatusNotifierItem::ItemStatus::Active);
//  setStatus(KStatusNotifierItem::ItemStatus::NeedsAttention);

    connect( this, &KStatusNotifierItem::activateRequested, this, &SysTrayXStatusNotifier::slotActivateRequested );
    connect( this, &KStatusNotifierItem::secondaryActivateRequested, this, &SysTrayXStatusNotifier::slotSecondaryActivateRequested );
}


/*
 *  Set the default icon type
 */
void    SysTrayXStatusNotifier::setDefaultIconType( Preferences::DefaultIconType icon_type )
{
    if( m_default_icon_type != icon_type )
    {
        /*
         *  Store the new value
         */
        m_default_icon_type = icon_type;

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}


/*
 *  Set the default icon mime
 */
void    SysTrayXStatusNotifier::setDefaultIconMime( const QString& icon_mime )
{
    if( m_default_icon_mime != icon_mime )
    {
        /*
         *  Store the new value
         */
        m_default_icon_mime = icon_mime;
    }
}


/*
 *  Set the default icon data
 */
void    SysTrayXStatusNotifier::setDefaultIconData( const QByteArray& icon_data )
{
    if( m_default_icon_data != icon_data )
    {
        /*
         *  Store the new value
         */
        m_default_icon_data = icon_data;

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}


/*
 *  Set the icon type
 */
void    SysTrayXStatusNotifier::setIconType( Preferences::IconType icon_type )
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
void    SysTrayXStatusNotifier::setIconMime( const QString& icon_mime )
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
 *  Set the icon data
 */
void    SysTrayXStatusNotifier::setIconData( const QByteArray& icon_data )
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
 *  Enable/disable number
 */
void    SysTrayXStatusNotifier::showNumber( bool state )
{
    if( m_show_number != state )
    {
        /*
         *  Store the new value
         */
        m_show_number = state;

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}


/*
 *  Set number color
 */
void    SysTrayXStatusNotifier::setNumberColor( const QString& color )
{
    if( m_number_color != color )
    {
        /*
         *  Store the new value
         */
        m_number_color = color;

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}


/*
 *  Set number size
 */
void    SysTrayXStatusNotifier::setNumberSize( int size )
{
    if( m_number_size != size )
    {
        /*
         *  Store the new value
         */
        m_number_size = size;

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}

/*
 *  Set the number of unread mails
 */
void    SysTrayXStatusNotifier::setUnreadMail( int unread_mail )
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
void    SysTrayXStatusNotifier::renderIcon()
{
    QPixmap pixmap;

    if( m_unread_mail > 0 )
    {
        switch( m_icon_type )
        {
            case Preferences::PREF_BLANK_ICON:
            {
                pixmap = QPixmap( ":/files/icons/blank-icon.png" );
                break;
            }

            case Preferences::PREF_NEWMAIL_ICON:
            {
                QIcon new_mail = QIcon::fromTheme("mail-unread", QIcon(":/files/icons/mail-unread.png"));
                pixmap = new_mail.pixmap( 256, 256 );
                break;
            }

            case Preferences::PREF_CUSTOM_ICON:
            {
                pixmap.loadFromData( m_icon_data );
                break;
            }

            case Preferences::PREF_NO_ICON:
            {
                QPixmap lookthrough( 256, 256 );
                lookthrough.fill( Qt::transparent );
                pixmap = lookthrough;
                break;
            }
        }
    }
    else
    {
        switch( m_default_icon_type )
        {
            case Preferences::PREF_DEFAULT_ICON_DEFAULT:
            {
                pixmap = QPixmap( ":/files/icons/Thunderbird.png" );
                break;
            }

            case Preferences::PREF_DEFAULT_ICON_HIDE:
            {
                pixmap = QPixmap();

                break;
            }

            case Preferences::PREF_DEFAULT_ICON_CUSTOM:
            {
                pixmap.loadFromData( m_default_icon_data );
                break;
            }
        }
    }

    if( m_show_number && ( m_unread_mail > 0 ) )
    {
        /*
         *  Paint the number
         */
        QPainter painter( &pixmap );

        painter.setFont( QFont("Sans") );

#if QT_VERSION < QT_VERSION_CHECK(5, 13, 0)
        double factor = pixmap.width() / ( 3 * painter.fontMetrics().width( "0" ) );
#else
        double factor = pixmap.width() / ( 3 * painter.fontMetrics().horizontalAdvance( "0" ) );
#endif
        QFont font = painter.font();
        font.setPointSizeF( font.pointSizeF() * ( factor * m_number_size / 10 ) );
        font.setBold( true );
        painter.setFont( font );

        painter.setPen( QColor( m_number_color ) );

        painter.drawText( pixmap.rect(), Qt::AlignCenter, QString::number( m_unread_mail ) );
    }

    /*
     *  Set the tray icon
     */
    setIconByPixmap( QIcon( pixmap ) );
}


/*
 *  Handle unread mail signal
 */
void    SysTrayXStatusNotifier::slotSetUnreadMail( int unread_mail )
{
    setUnreadMail( unread_mail );
}


/*
 *  Handle the default icon type change signal
 */
void    SysTrayXStatusNotifier::slotDefaultIconTypeChange()
{
    setDefaultIconType( m_pref->getDefaultIconType() );
}


/*
 *  Handle the default icon data change signal
 */
void    SysTrayXStatusNotifier::slotDefaultIconDataChange()
{
    setDefaultIconMime( m_pref->getDefaultIconMime() );
    setDefaultIconData( m_pref->getDefaultIconData() );
}


/*
 *  Handle the icon type change signal
 */
void    SysTrayXStatusNotifier::slotIconTypeChange()
{
    setIconType( m_pref->getIconType() );
}


/*
 *  Handle the icon data change signal
 */
void    SysTrayXStatusNotifier::slotIconDataChange()
{
    setIconMime( m_pref->getIconMime() );
    setIconData( m_pref->getIconData() );
}


/*
 *  Handle the enable number state change signal
 */
void    SysTrayXStatusNotifier::slotShowNumberChange()
{
    showNumber( m_pref->getShowNumber() );
}


/*
 *  Handle the number color change signal
 */
void    SysTrayXStatusNotifier::slotNumberColorChange()
{
    setNumberColor( m_pref->getNumberColor() );
}


/*
 *  Handle the number size change signal
 */
void    SysTrayXStatusNotifier::slotNumberSizeChange()
{
    setNumberSize( m_pref->getNumberSize() );
}


/*
 *  Handle activate request of the notification icon
 */
void    SysTrayXStatusNotifier::slotActivateRequested( bool active, const QPoint &pos )
{
    Q_UNUSED( active )
    Q_UNUSED( pos )

    emit signalShowHide();
}


/*
 *  Handle secondary activate request of the notification icon
 */
void    SysTrayXStatusNotifier::slotSecondaryActivateRequested( const QPoint &pos )
{
    Q_UNUSED( pos )

    emit signalShowHide();
}
