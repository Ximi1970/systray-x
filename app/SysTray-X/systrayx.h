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
         * @brief createTrayIcon. Create the system tray icon.
         */
        void    createTrayIcon();

        /**
         * @brief createActions. Create the menu actions.
         */
        void    createMenu();


        void    showTrayIcon();
        void    hideTrayIcon();


    signals:

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

    private slots:

        void    slotShow();
        void    slotHide();

        /**
         * @brief slotUnreadMail. Handle unread mail signal.
         *
         *  @param unread_mail  Number of unread mail.
         */
        void    slotUnreadMail( int unread_mail );

        /**
         * @brief slotAddOnShutdown. Handle shutdown request from the add-on.
         */
        void    slotAddOnShutdown();

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
};

#endif // SYSTRAYX_H
