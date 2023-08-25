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
         * @brief setMailCount. Set the number of unread/new mails.
         *
         *  @param unread_mail  The number of unread mails.
         *  @param new_mail  The number of new mails.
         */
        void    setMailCount( int unread_mail, int new_mail );

    private:

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
         * @brief slotShowNumberChange. Slot for handling show number change signals.
         */
        void    slotShowNumberChange();

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
         * @brief slotThemeChange. Slot for handling theme change signals.
         */
        void    slotThemeChange();

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
