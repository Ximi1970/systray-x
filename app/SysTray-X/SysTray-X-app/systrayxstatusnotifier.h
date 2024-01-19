#include <QtGlobal>
#if defined( Q_OS_UNIX ) && defined( KDE_INTEGRATION )

#ifndef SYSTRAYXSTATUSNOTIFIER_H
#define SYSTRAYXSTATUSNOTIFIER_H


/*
 *	Local includes
 */
#include "preferences.h"

/*
 *	Qt includes
 */
#include <KStatusNotifierItem>
#include <QImage>
#include <QPixmap>

/*
 *	Predefines
 */
class SysTrayXLink;


/**
 * @brief The SysTrayXStatusNotifier class. The system tray icon.
 */
class SysTrayXStatusNotifier : public KStatusNotifierItem
{
    Q_OBJECT

    public:

        /**
         * @brief SysTrayXStatusNotifier. Constructor.
         *
         *  @param parent   My parent.
         */
        SysTrayXStatusNotifier( SysTrayXLink* link, Preferences* pref, QObject* parent = nullptr );

        /**
         * @brief setDefaultIconType. Set the sytem tray default icon type.
         *
         *  @param icon_type    The icon type
         */
        void    setDefaultIconType( Preferences::DefaultIconType icon_type );

        /**
         * @brief setDefaultIconMime. Set the sytem tray icon mime.
         *
         *  @param icon_mime    The icon mime
         */
        void    setDefaultIconMime( const QString& icon_mime );

        /**
         * @brief setDefaultIconData. Set the custom icon data.
         *
         *  @param icon_data    The icon data.
         */
        void    setDefaultIconData( const QByteArray& icon_data );

        /**
         * @brief setHideDefaultIcon. Set hide default icon.
         *
         *  @param hide    The state
         */
        void    setHideDefaultIcon( bool hide );

        /**
         * @brief setIconType. Set the sytem tray icon type.
         *
         *  @param icon_type    The icon type
         */
        void    setIconType( Preferences::IconType icon_type );

        /**
         * @brief setIconMime. Set the sytem tray icon mime.
         *
         *  @param icon_mime    The icon mime
         */
        void    setIconMime( const QString& icon_mime );

        /**
         * @brief setIconData. Set the custom icon data.
         *
         *  @param icon_data    The icon data.
         */
        void    setIconData( const QByteArray& icon_data );

        /**
         * @brief showNumber. Set the show number state.
         *
         *  @param state    Show / hide.
         */
        void    showNumber( bool state );

        /**
         * @brief showNewIndicator. Set the show new indicator state.
         *
         *  @param state    Show / hide.
         */
        void    showNewIndicator( bool state );

        /**
         * @brief setNumberColor. Set the number color.
         *
         *  @param color    The color.
         */
        void    setNumberColor( const QString& color );

        /**
         * @brief setNumberSize. Set the number size.
         *
         *  @param size    The size.
         */
        void    setNumberSize( int size );

        /**
         * @brief setNumberAlignment. Set the number alignment.
         *
         * @param alignment     The alignment.
         */
        void    setNumberAlignment( int alignment );

        /**
         * @brief setNumberMargins. Set the number margins.
         *
         * @param margins     The margins.
         */
        void    setNumberMargins( QMargins margins );

        /**
         * @brief setNewIndicatorType. Set the new indicator type.
         *
         *  @param new_indicator_type   The new indicator type.
         */
        void    setNewIndicatorType( Preferences::NewIndicatorType new_indicator_type );

        /**
         * @brief setNewShadeColor. Set the new shade color.
         *
         *  @param color    The color.
         */
        void    setNewShadeColor( const QString& color );

        /**
         * @brief setMailCount. Set the number of unread/new mails.
         *
         *  @param unread_mail  The number of unread mails.
         *  @param new_mail  The number of new mails.
         */
        void    setMailCount( int unread_mail, int new_mail );

    private:

        /**
         * @brief renderBase. Set the base pixmaps for the icon.
         */
        void    renderBase();

        /**
         * @brief shade. Shade a pixmap
         *
         *  @param pixmap   Pixmap to shade.
         */
        void    shade( QPixmap& pixmap );

        /**
         * @brief indicator. Add a new indicator to the icon.
         *
         *  @param pixmap   Pixmap to alter.
         */
        void    indicator( QPixmap& pixmap );

        /**
         * @brief setIcon. Set a new rendered icon.
         */
        void    renderIcon();

        /**
         * @brief showIcon. Show the icon.
         */
        void    showIcon();

    signals:

        /**
         * @brief signalShowHide. Signal show / hide window.
         */
        void    signalShowHide();

    public slots:

        /**
         * @brief slotMailCount. Slot for handling unread/new mail signals.
         *
         *  @param unread_mail  The number of unread mails.
         *  @param new_mail     The number of new mails.
         */
        void    slotMailCount( int unread_mail, int new_mail );

        /**
         * @brief slotDefaultIconTypeChange. Slot for handling default icon type change signals.
         */
        void    slotDefaultIconTypeChange();

        /**
         * @brief slotDefaultIconDataChange. Slot for handling default icon data change signals.
         */
        void    slotDefaultIconDataChange();

        /**
         * @brief slotHideDefaultIconChange. Slot for handling hide default icon change signals.
         */
        void    slotHideDefaultIconChange();

        /**
         * @brief slotIconTypeChange. Slot for handling icon type change signals.
         */
        void    slotIconTypeChange();

        /**
         * @brief slotIconDataChange. Slot for handling icon data change signals.
         */
        void    slotIconDataChange();

        /**
         * @brief slotInvertIconChange. Slot for handling invert icon change signals.
         */
        void    slotInvertIconChange();

        /**
         * @brief slotShowNumberChange. Slot for handling show number change signals.
         */
        void    slotShowNumberChange();

        /**
         * @brief slotShowNewIndicatorChange. Slot for handling show new indicator change signals.
         */
        void    slotShowNewIndicatorChange();

        /**
         * @brief slotNumberColorChange. Slot for handling number color change signals.
         */
        void    slotNumberColorChange();

        /**
         * @brief slotNumberSizeChange. Slot for handling number size change signals.
         */
        void    slotNumberSizeChange();

        /**
         * @brief slotNumberAlignmentChange. Slot for handling number alignment change signals.
         */
        void    slotNumberAlignmentChange();

        /**
         * @brief slotNumberMarginsChange. Slot for handling number margins change signals.
         */
        void    slotNumberMarginsChange();

        /**
         * @brief slotNewIndicatorTypeChange. Slot for handling new indicator type change signals.
         */
        void    slotNewIndicatorTypeChange();

        /**
         * @brief slotNewShadeColorChange. Slot for handling new shade color change signals.
         */
        void    slotNewShadeColorChange();

    private slots:

        /**
         * @brief slotActivateRequested. Handle a activate request of the notifier.
         *
         *  @param active       Window request show / hide.
         *  @param pos          Click position.
         */
        void    slotActivateRequested( bool active, const QPoint &pos );

        /**
         * @brief slotSecondaryActivateRequested. Handle a secondary activate request of the notifier.
         *
         *  @param pos          Click position.
         */
        void    slotSecondaryActivateRequested( const QPoint &pos );

    private:

        /**
         * @brief m_link.   Pointer to the link.
         */
        SysTrayXLink*   m_link;

        /**
         * @brief m_pref    Pointer to the preferences storage.
         */
        Preferences*    m_pref;

        /**
         * @brief m_pixmap_count    Pixmap to be used when counting.
         */
        QPixmap m_pixmap_count;

        /**
         * @brief m_pixmap_clean    Pixmap to be used when there is no new mail.
         */
        QPixmap m_pixmap_clean;

        /**
         * @brief m_image_indicator    Image to be used as new mail indicator.
         */
        QImage m_image_indicator;

        /**
         * @brief m_default_icon_type. Storage for the default icon type.
         */
        Preferences::DefaultIconType   m_default_icon_type;

        /**
         * @brief m_default_icon_mime. Storage for the default icon mime.
         */
        QString m_default_icon_mime;

        /**
         * @brief m_default_icon_data. Storage for the default icon.
         */
        QByteArray  m_default_icon_data;

        /**
         * @brief m_hide_default_icon. Storage for the hide default icon state.
         */
        bool    m_hide_default_icon;

        /**
         * @brief m_icon_type. Storage for the icon type.
         */
        Preferences::IconType   m_icon_type;

        /**
         * @brief m_icon_mime. Storage for the icon mime.
         */
        QString m_icon_mime;

        /**
         * @brief m_icon_data. Storage for the icon.
         */
        QByteArray  m_icon_data;

        /**
         * @brief m_show_number. Show the unread/new mail count.
         */
        bool m_show_number;

        /**
         * @brief m_show_new_indicator. Show the new indicator.
         */
        bool m_show_new_indicator;

        /**
         * @brief m_number_color. Color of the unread/new mail number.
         */
        QString m_number_color;

        /**
         * @brief m_number_size. Size of the unread/new mail number.
         */
        int m_number_size;

        /**
         * @brief m_number_alignment. The number alignment.
         */
        int m_number_alignment;

        /**
         * @brief m_margins. The number margins.
         */
        QMargins m_number_margins;

        /**
         * @brief m_new_indicator_type. The new indicator type.
         */
        Preferences::NewIndicatorType   m_new_indicator_type;

        /**
         * @brief m_new_shade_color. Color of the new shade.
         */
        QString m_new_shade_color;

        /**
         * @brief m_unread_mail. Storage for the number of unread mails.
         */
        int m_unread_mail;

        /**
         * @brief m_new_mail. Storage for the number of new mails.
         */
        int m_new_mail;
};

#endif // SYSTRAYXSTATUSNOTIFIER_H

#endif
