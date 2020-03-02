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
         * @brief slotIconTypeChange. Slot for handling icon type change signals.
         */
        void    slotIconTypeChange();

        /**
         * @brief slotIconDataChange. Slot for handling icon data change signals.
         */
        void    slotIconDataChange();

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
         * @brief m_unread_mail. Storage for the number of unread mails.
         */
        int m_unread_mail;
};

#endif // SYSTRAYXICON_H
