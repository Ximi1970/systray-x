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
SysTrayXIcon::SysTrayXIcon( SysTrayXLink* link, Preferences* pref, QObject* parent )
    : QSystemTrayIcon( parent )
{
    /*
     *  Initialize
     */
    m_link = link;
    m_pref = pref;

    m_unread_mail = 0;
    m_new_mail = 0;

    m_pixmap_clean = QPixmap();
    m_pixmap_count = QPixmap();
    m_image_indicator = QImage();

    m_show_number = m_pref->getShowNumber();
    m_show_new_indicator = m_pref->getShowNewIndicator();
    m_number_color = m_pref->getNumberColor();
    m_number_size = m_pref->getNumberSize();
    m_number_alignment = Qt::AlignHCenter | Qt::AlignVCenter;
    setNumberAlignment( m_pref->getNumberAlignment() );
    m_number_margins = m_pref->getNumberMargins();
    m_new_shade_color = m_pref->getNewShadeColor();

    setToolTip( tr( "SysTray-X: Thunderbird add-on companion app" ) );

    renderBase();

    connect( this, &QSystemTrayIcon::activated, this, &SysTrayXIcon::slotIconActivated );
}


/*
 *  Set the default icon type
 */
void    SysTrayXIcon::setDefaultIconType( Preferences::DefaultIconType icon_type )
{
    if( m_default_icon_type != icon_type )
    {
        /*
         *  Store the new value
         */
        m_default_icon_type = icon_type;

        /*
         *  Set base params
         */
        renderBase();

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}


/*
 *  Set the default icon mime
 */
void    SysTrayXIcon::setDefaultIconMime( const QString& icon_mime )
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
void    SysTrayXIcon::setDefaultIconData( const QByteArray& icon_data )
{
    if( m_default_icon_data != icon_data )
    {
        /*
         *  Store the new value
         */
        m_default_icon_data = icon_data;

        /*
         *  Set base params
         */
        renderBase();

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}


/*
 *  Set the icon type
 */
void    SysTrayXIcon::setIconType( Preferences::IconType icon_type )
{
    if( icon_type != m_icon_type )
    {
        /*
         *  Store the new value
         */
        m_icon_type = icon_type;

        /*
         *  Set base params
         */
        renderBase();

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}


/*
 *  Set the icon mime
 */
void    SysTrayXIcon::setIconMime( const QString& icon_mime )
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
void    SysTrayXIcon::setIconData( const QByteArray& icon_data )
{
    if( m_icon_data != icon_data )
    {
        /*
         *  Store the new value
         */
        m_icon_data = icon_data;

        /*
         *  Set base params
         */
        renderBase();

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}


/*
 *  Enable/disable number
 */
void    SysTrayXIcon::showNumber( bool state )
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
 *  Enable/disable new indicator
 */
void    SysTrayXIcon::showNewIndicator( bool state )
{
    if( m_show_new_indicator != state )
    {
        /*
         *  Store the new value
         */
        m_show_new_indicator = state;

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}


/*
 *  Set number color
 */
void    SysTrayXIcon::setNumberColor( const QString& color )
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
void    SysTrayXIcon::setNumberSize( int size )
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
 *  Set number alignment
 */
void    SysTrayXIcon::setNumberAlignment( int alignment )
{
    int alignment_qt;
    switch( alignment )
    {
        case 0: alignment_qt = Qt::AlignTop | Qt::AlignLeft; break;
        case 1: alignment_qt = Qt::AlignTop | Qt::AlignHCenter; break;
        case 2: alignment_qt = Qt::AlignTop | Qt::AlignRight; break;

        case 3: alignment_qt = Qt::AlignVCenter | Qt::AlignLeft; break;
        case 4: alignment_qt = Qt::AlignVCenter | Qt::AlignHCenter; break;
        case 5: alignment_qt = Qt::AlignVCenter | Qt::AlignRight; break;

        case 6: alignment_qt = Qt::AlignBottom | Qt::AlignLeft; break;
        case 7: alignment_qt = Qt::AlignBottom | Qt::AlignHCenter; break;
        case 8: alignment_qt = Qt::AlignBottom | Qt::AlignRight; break;

        default:
        {
            alignment_qt = Qt::AlignHCenter | Qt::AlignVCenter;
            break;
        }
    }

    if( m_number_alignment != alignment_qt )
    {
        /*
         *  Store the new value
         */
        m_number_alignment = alignment_qt;

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}


/*
 *  Set number margins
 */
void    SysTrayXIcon::setNumberMargins( QMargins margins )
{
    if( m_number_margins != margins )
    {
        /*
         *  Store the new value
         */
        m_number_margins = margins;

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}


/*
 *  Set the new indicator type
 */
void    SysTrayXIcon::setNewIndicatorType( Preferences::NewIndicatorType new_indicator_type )
{
    if(  m_new_indicator_type != new_indicator_type )
    {
        /*
         *  Store the new value
         */
        m_new_indicator_type = new_indicator_type;

        /*
         *  Set base params
         */
        renderBase();

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}

/*
 *  Set new shade color
 */
void    SysTrayXIcon::setNewShadeColor( const QString& color )
{
    if( m_new_shade_color != color )
    {
        /*
         *  Store the new value
         */
        m_new_shade_color = color;

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}


/*
 *  Set the number of unread/new mails
 */
void    SysTrayXIcon::setMailCount( int unread_mail, int new_mail )
{
    if( m_unread_mail != unread_mail || m_new_mail != new_mail )
    {
        /*
         *  Store the new value
         */
        m_unread_mail = unread_mail;
        m_new_mail = new_mail;

        /*
         *  Render and set a new icon in the tray
         */
        renderIcon();
    }
}


/*
 *  Set the base for rendering
 */
void    SysTrayXIcon::renderBase()
{
    /*
     * Set the clean icon
     */
    switch( m_default_icon_type )
    {
        case Preferences::PREF_DEFAULT_ICON_DEFAULT:
        {
            QString version = m_pref->getBrowserVersion();

            if( version.section( '.', 0, 0 ).toInt() < 115 )
            {
                m_pixmap_clean = QPixmap( ":/files/icons/Thunderbird.png" );
            }
            else
            {
                m_pixmap_clean = QPixmap( ":/files/icons/Thunderbird115.png" );
            }
            break;
        }

        case Preferences::PREF_DEFAULT_ICON_HIDE:
        {
            m_pixmap_clean = QPixmap();
            break;
        }

        case Preferences::PREF_DEFAULT_ICON_CUSTOM:
        {
            m_pixmap_clean.loadFromData( m_default_icon_data );
            break;
        }
    }

    /*
     * Set the count icon
     */
    switch( m_icon_type )
    {
        case Preferences::PREF_BLANK_ICON:
        {
            Preferences::Theme theme = m_pref->getTheme();

            if( theme == Preferences::PREF_THEME_LIGHT )
            {
                m_pixmap_count = QPixmap( ":/files/icons/blank-icon.png" );
            }
            else
            {
                m_pixmap_count = QPixmap( ":/files/icons/blank-icon-dark.png" );
            }

            break;
        }

        case Preferences::PREF_NEWMAIL_ICON:
        {
            Preferences::Theme theme = m_pref->getTheme();

            QIcon new_mail = QIcon::fromTheme("mail-unread", QIcon(":/files/icons/mail-unread.png"));

            if( theme == Preferences::PREF_THEME_LIGHT )
            {
                m_pixmap_count = new_mail.pixmap( 256, 256 );
            }
            else
            {
                QImage new_mail_image = new_mail.pixmap( 256, 256 ).toImage();
                new_mail_image.invertPixels();
                m_pixmap_count = QPixmap::fromImage( new_mail_image );
            }

            break;
        }

        case Preferences::PREF_CUSTOM_ICON:
        {
            m_pixmap_count.loadFromData( m_icon_data );
            break;
        }

        case Preferences::PREF_NO_ICON:
        {
            QPixmap lookthrough( 256, 256 );
            lookthrough.fill( Qt::transparent );
            m_pixmap_count = lookthrough;
            break;
        }

        case Preferences::PREF_TB_ICON:
        {
            QString version = m_pref->getBrowserVersion();

            if( version.section( '.', 0, 0 ).toInt() < 115 )
            {
                m_pixmap_count = QPixmap( ":/files/icons/Thunderbird.png" );
            }
            else
            {
                m_pixmap_count = QPixmap( ":/files/icons/Thunderbird115.png" );
            }
            break;
        }
    }

    /*
     *  Set the new indicator
     */
    switch( m_pref->getNewIndicatorType() )
    {
        case Preferences::PREF_NEW_INDICATOR_ROUND:
        {
            m_image_indicator = QImage( ":/files/icons/new-indicator-round.png" );
            break;
        }

        case Preferences::PREF_NEW_INDICATOR_STAR:
        {
            m_image_indicator = QImage( ":/files/icons/new-indicator-star-close.png" );
            break;
        }

        default:
        {
            m_image_indicator = QImage();
            break;
        }
    }
}


/*
 *  Shade the pixmap
 */
void    SysTrayXIcon::shade( QPixmap& pixmap )
{
    QPainter painter( &pixmap );
    painter.setCompositionMode( painter.CompositionMode_Overlay );
    painter.fillRect( pixmap.rect(), QColor( m_new_shade_color ) );
    painter.end();
}


/*
 *  Indicator on the pixmap
 */
void    SysTrayXIcon::indicator( QPixmap& pixmap )
{
    int size_x = pixmap.width() / 2;
    int size_y = pixmap.width() / 2;
    QRect topRight( size_x, 0, size_x, size_y );

    QPainter painter( &pixmap );
    painter.drawImage( topRight, m_image_indicator );
    painter.end();
}


/*
 *  Set and render the icon in the system tray
 */
void    SysTrayXIcon::renderIcon()
{
    QPixmap pixmap;
    int count;

    if( m_pref->getCountType() == Preferences::PREF_COUNT_UNREAD )
    {
        count = m_unread_mail;
    }
    else
    {
        count = m_new_mail;
    }

    if( count > 0 )
    {
        pixmap = m_pixmap_count;
    }
    else
    {
        pixmap = m_pixmap_clean;
    }

    if( m_show_new_indicator && m_new_mail > 0 )
    {
        if( m_pref->getNewIndicatorType() == Preferences::PREF_NEW_INDICATOR_SHADE )
        {
            shade( pixmap );
        }
        else
        {
            indicator( pixmap );
        }
    }

    if( m_show_number && count > 0 )
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

        QRect bounding = pixmap.rect().adjusted( m_number_margins.left(), m_number_margins.top(),
                                                 -m_number_margins.right(), -m_number_margins.bottom());

        painter.drawText( bounding, m_number_alignment, QString::number( count ) );
    }

    /*
     *  Set the tray icon
     */
    QSystemTrayIcon::setIcon( QIcon( pixmap ) );
}


/*
 *  Handle mail count signal
 */
void    SysTrayXIcon::slotMailCount( int unread_mail, int new_mail )
{
    setMailCount( unread_mail, new_mail );
}


/*
 *  Handle the default icon type change signal
 */
void    SysTrayXIcon::slotDefaultIconTypeChange()
{
    setDefaultIconType( m_pref->getDefaultIconType() );
}


/*
 *  Handle the default icon data change signal
 */
void    SysTrayXIcon::slotDefaultIconDataChange()
{
    setDefaultIconMime( m_pref->getDefaultIconMime() );
    setDefaultIconData( m_pref->getDefaultIconData() );
}


/*
 *  Handle the icon type change signal
 */
void    SysTrayXIcon::slotIconTypeChange()
{
    setIconType( m_pref->getIconType() );
}


/*
 *  Handle the icon data change signal
 */
void    SysTrayXIcon::slotIconDataChange()
{
    setIconMime( m_pref->getIconMime() );
    setIconData( m_pref->getIconData() );
}


/*
 *  Handle the theme change signal
 */
void    SysTrayXIcon::slotThemeChange()
{
    renderBase();
    renderIcon();
}


/*
 *  Handle the show number state change signal
 */
void    SysTrayXIcon::slotShowNumberChange()
{
    showNumber( m_pref->getShowNumber() );
}


/*
 *  Handle the show new indicator state change signal
 */
void    SysTrayXIcon::slotShowNewIndicatorChange()
{
    showNewIndicator( m_pref->getShowNewIndicator() );
}


/*
 *  Handle the number color change signal
 */
void    SysTrayXIcon::slotNumberColorChange()
{
    setNumberColor( m_pref->getNumberColor() );
}


/*
 *  Handle the number size change signal
 */
void    SysTrayXIcon::slotNumberSizeChange()
{
    setNumberSize( m_pref->getNumberSize() );
}


/*
 *  Handle the number alignment change signal
 */
void    SysTrayXIcon::slotNumberAlignmentChange()
{
    setNumberAlignment( m_pref->getNumberAlignment() );
}


/*
 *  Handle the number margins change signal
 */
void    SysTrayXIcon::slotNumberMarginsChange()
{
    setNumberMargins( m_pref->getNumberMargins() );
}


/*
 *  Handle the new indicator type change signal
 */
void    SysTrayXIcon::slotNewIndicatorTypeChange()
{
    setNewIndicatorType( m_pref->getNewIndicatorType() );
}


/*
 *  Handle the new shade color change signal
 */
void    SysTrayXIcon::slotNewShadeColorChange()
{
    setNewShadeColor( m_pref->getNewShadeColor() );
}


/*
 *  Handle activation of the tray icon
 */
void    SysTrayXIcon::slotIconActivated( QSystemTrayIcon::ActivationReason reason )
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::MiddleClick:
        {
            //  Clicked
            emit signalShowHide();
            break;
        }

        case QSystemTrayIcon::DoubleClick:
        {
            break;
        }

        default:
        {
            break;
        }
    }
}
