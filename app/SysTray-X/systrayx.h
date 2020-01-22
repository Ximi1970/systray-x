#ifndef SYSTRAYX_H
#define SYSTRAYX_H

/*
 *	Local includes
 */
#include "ui_debugwidget.h"
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
        void getPreferences();

        /**
         * @brief iconActivated
         * @param reason
         */
        void iconActivated( QSystemTrayIcon::ActivationReason reason );

        /**
         * @brief createTrayIcon. Create the system tray icon.
         */
        void createTrayIcon();

        /**
         * @brief createActions. Create the menu actions.
         */
        void createActions();

    signals:

        /**
         * @brief signalWriteMessage
         *
         * @param message
         */
        void signalWriteMessage( QByteArray message );

    private slots:

    private:

        /**
         * @brief m_preferences. Pointer to the preferences storage.
         */
        Preferences *m_preferences;

        /**
         * @brief m_debug
         */
        DebugWidget *m_debug;

        /**
         * @brief m_link. Pointer to the link object.
         */
        SysTrayXLink *m_link;

        /**
         * @brief m_pref_dialog. Pointer to the preferences dialog.
         */
        PreferencesDialog *m_pref_dialog;

        /**
         * @brief m_tray_icon. Pointer to the system tray icon.
         */
        SysTrayXIcon *m_tray_icon;

        /**
         * @brief m_tray_icon_menu. Pointer to the tray icon menu.
         */
        QMenu *m_tray_icon_menu;

        /**
         * @brief m_xxxx_action. Pointer to the menu actions.
         */
        QAction *m_pref_action;
        QAction *m_quit_action;
};

#endif // SYSTRAYX_H
