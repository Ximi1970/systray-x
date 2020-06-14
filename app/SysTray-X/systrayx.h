#ifndef SYSTRAYX_H
#define SYSTRAYX_H

/*
 *	Local includes
 */
#include "ui_debugwidget.h"
#include "ui_aboutdialog.h"
#include "preferences.h"

/*
 *	Qt includes
 */
#include <QObject>
#include <QTranslator>
#include <QSystemTrayIcon>

/*
 *	Predefines
 */
class QAction;

class DebugWidget;
class PreferencesDialog;
class SysTrayXIcon;
class SysTrayXLink;
class WindowCtrl;

class SysTrayXStatusNotifier;

/**
 * @brief The SysTrayX class
 */
class SysTrayX : public QObject
{
    Q_OBJECT

    public:

        static const QString JSON_PREF_REQUEST;

    public:

        /**
         * @brief SysTrayX. Constructor.
         *
         *  @param parent   My parent.
         */
        explicit SysTrayX( QObject *parent = nullptr );

    private:

        /**
         * @brief SysTrayX::getPreferences
         */
        void    getPreferences();

        /**
         * @brief createActions. Create the menu actions.
         */
        void    createMenu();

        /**
         * @brief destroyMenu. Destroy the menu.
         */
        void    destroyMenu();

        /**
         * @brief showTrayIcon. Create and show the icon.
         */
        void    showTrayIcon();

        /**
         * @brief hideTrayIcon. Hide and destroy the icon.
         */
        void    hideTrayIcon();

#ifdef Q_OS_UNIX

        /**
         * @brief showKdeTrayIcon. Create and show the KDE icon.
         */
        void    showKdeTrayIcon();

        /**
         * @brief hideKdeTrayIcon. Hide and destroy the KDE icon.
         */
        void    hideKdeTrayIcon();

#endif

        /**
         * @brief resendUnreadMail. Send another unread mail signal to the icon.
         */
        void    resendUnreadMail();

    signals:

        /**
         * @brief signalUnreadMail. Signal numder of unread mails.
         *
         * @param unreadMail    The number of unread mails.
         */
        void    signalUnreadMail( int unread_mail );

        /**
         * @brief signalWriteMessage
         *
         * @param message
         */
        void    signalWriteMessage( QByteArray message );

        /**
         * @brief signalClose. Signal close all TB windows.
         */
        void    signalClose();

        /**
         * @brief signalConsole. Send a console message.
         *
         *  @param message      The message.
         */
        void    signalConsole( QString message );

    public slots:

        /**
         * @brief slotSetUnreadMail. Handle the unred mail signal.
         *
         *  @param unread   Number of unread mail.
         */
        void    slotSetUnreadMail( int unread );

        /**
         * @brief slotLoadLanguage. Handle locale change signal.
         *
         *  @param locale   The requested locale.
         */
        void    slotLoadLanguage( QString locale );

    private slots:

        /**
         * @brief slotSelectIconObject. Select the prefered icon.
         *
         *  @param state    The state.
         */
        void    slotSelectIconObjectPref();
        void    slotSelectIconObject( bool state );

        /**
         * @brief slotAddOnShutdown. Handle shutdown request from the add-on.
         */
        void    slotAddOnShutdown();

        /**
         * @brief slotErrorAddOnShutdown. Handle shutdown request from the add-on.
         */
        void    slotErrorAddOnShutdown();

        /**
         * @brief slotShutdown. Handle shutdown request from the menu.
         */
        void    slotShutdown();

        /**
         * @brief slotAbout. Handle about request from the menu.
         */
        void    slotAbout();

        /**
         * @brief slotVersion. Handle version info from addon.
         */
        void    slotVersion( QString version );

    private:

        /**
         * @brief m_preferences. Pointer to the preferences storage.
         */
        Preferences*    m_preferences;

        /**
         * @brief m_debug
         */
        DebugWidget*    m_debug;

        /**
         * @brief m_win_ctrl. Pointer to the window control.
         */
        WindowCtrl*     m_win_ctrl;

        /**
         * @brief m_link. Pointer to the link object.
         */
        SysTrayXLink*   m_link;

        /**
         * @brief m_pref_dialog. Pointer to the preferences dialog.
         */
        PreferencesDialog*  m_pref_dialog;

        /**
         * @brief m_tray_icon. Pointer to the system tray icon.
         */
        SysTrayXIcon*   m_tray_icon;

        /**
         * @brief m_kde_tray_icon. Pointer to the KDE system tray icon.
         */
        SysTrayXStatusNotifier*   m_kde_tray_icon;

        /**
         * @brief m_tray_icon_menu. Pointer to the tray icon menu.
         */
        QMenu*  m_tray_icon_menu;

        /**
         * @brief m_xxxx_action. Pointer to the menu actions.
         */
        QAction*    m_showhide_action;
        QAction*    m_pref_action;
        QAction*    m_about_action;
        QAction*    m_quit_action;

        /**
         * @brief m_unread_mail. Number of unread mails
         */
        int m_unread_mail;

        /**
         * @brief m_translator. The translator object.
         */
        QTranslator m_translator;

        /**
         * @brief m_locale. Storage for the current locale.
         */
        QString m_locale;
};

#endif // SYSTRAYX_H
