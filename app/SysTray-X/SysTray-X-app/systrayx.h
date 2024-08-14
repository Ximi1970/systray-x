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

class Shortcut;


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
         * @brief resendMailCount. Send another unread/new mail signal to the icon.
         */
        void    resendMailCount();

    signals:

        /**
         * @brief signalMailCount. Signal numder of unread/new mails.
         *
         * @param unread_mail   The number of unread mails.
         * @param new_mail      The number of new mails.
         */
        void    signalMailCount( int unread_mail, int new_mail );

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
         * @brief slotMailCount. Handle the mail count signal.
         *
         *  @param unread_mail  Number of unread mail.
         *  @param new_mail     Number of new mail.
         */
        void    slotMailCount( int unread_mail, int new_mail );

        /**
         * @brief slotLoadLanguage. Handle locale change signal.
         *
         *  @param locale   The requested locale.
         */
        void    slotLoadLanguage( QString locale );

        /**
         * @brief slotStartApp. Handle a start app launch request.
         */
        void    slotStartApp();

        /**
         * @brief slotCloseApp. Handle a close app launch request.
         */
        void    slotCloseApp();

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

        /**
         * @brief slotShowHideShortcutChange. Handle a show / hide shortcut change.
         */
        void    slotShowHideShortcutChange();

        /**
         * @brief slotNewMessage. Handle a new message request.
         */
        void    slotNewMessage();

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
        QAction*    m_new_action;
        QAction*    m_pref_action;
        QAction*    m_about_action;
        QAction*    m_quit_action;

        /**
         * @brief m_unread_mail. Number of unread mails
         */
        int m_unread_mail;

        /**
         * @brief m_new_mail. Number of new mails
         */
        int m_new_mail;

        /**
         * @brief m_translator. The translator object.
         */
        QTranslator m_translator;

        /**
         * @brief m_locale. Storage for the current locale.
         */
        QString m_locale;

        /**
         * @brief m_show_hide_shortcut. Storage for the show / hide shortcut.
         */
        Shortcut* m_show_hide_shortcut;
};

#endif // SYSTRAYX_H
