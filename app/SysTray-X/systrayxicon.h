#ifndef SYSTRAYXICON_H
#define SYSTRAYXICON_H

/*
 *	Local includes
 */
#include "preferences.h"

/*
 *	Qt includes
 */
#include <QSystemTrayIcon>

/*
 *	Predefines
 */
class SysTrayXLink;


/**
 * @brief The systrayxtray class. The system tray icon.
 */
class SysTrayXIcon : public QSystemTrayIcon
{
    Q_OBJECT

    public:

        /**
         * @brief SysTrayXIcon. Constructor.
         *
         *  @param parent   My parent.
         */
        SysTrayXIcon( SysTrayXLink* link, Preferences* pref, QObject* parent = nullptr );

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
         * @brief setUnreadMail. Set the number of unread mails.
         *
         *  @param unread_mail  The number of unread mails.
         */
        void    setUnreadMail( int unread_mail );

    private:

        /**
         * @brief setIcon. Set a new rendered icon.
         */
        void    renderIcon();

    signals:

        /**
         * @brief signalShowHide. Signal show / hide window.
         */
        void    signalShowHide();

   public slots:

        /**
         * @brief slotSetUnreadMail. Slot for handling unread mail signals.
         *
         *  @param unread_mail  The number of unread mails.
         */
        void    slotSetUnreadMail( int unread_mail );

        /**
         * @brief slotDefaultIconTypeChange. Slot for handling default icon type change signals.
         */
        void    slotDefaultIconTypeChange();

        /**
         * @brief slotDefaultIconDataChange. Slot for handling default icon data change signals.
         */
        void    slotDefaultIconDataChange();

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

    private slots:

        /**
         * @brief slotIconActivated. Hnadle activation signal of the tray icon.
         *
         *  @param reason   Activation reason.
         */
        void    slotIconActivated( QSystemTrayIcon::ActivationReason reason );

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
         * @brief m_show_number. Show the unread mail count.
         */
        bool m_show_number;

        /**
         * @brief m_number_color. Color of the unread mail number.
         */
        QString m_number_color;

        /**
         * @brief m_unread_mail. Storage for the number of unread mails.
         */
        int m_unread_mail;
};

#endif // SYSTRAYXICON_H
