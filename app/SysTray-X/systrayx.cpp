#include "systrayx.h"

/*
 *	Local includes
 */
#include "debugwidget.h"
#include "preferencesdialog.h"
#include "systrayxlink.h"
#include "systrayxicon.h"

/*
 *	Qt includes
 */
#include <QCoreApplication>
#include <QMenu>

/*
 *  Constants
 */
const QString SysTrayX::JSON_PREF_REQUEST = "{\"preferences\":{}}";


/*
 *  Constructor
 */
SysTrayX::SysTrayX( QObject *parent ) : QObject( parent )
{
    /*
     *  Setup preferences storage
     */
    m_preferences = new Preferences();

    /*
     *  Setup the link
     */
    m_link = new SysTrayXLink( m_preferences );

    /*
     *  Setup preferences dialog
     */
    m_pref_dialog = new PreferencesDialog( m_link, m_preferences );

    /*
     *  Setup tray icon
     */
    createTrayIcon();
    m_tray_icon->show();

    /*
     *  Setup debug window
     */
    m_debug = new DebugWidget( m_preferences );
    if( m_preferences->getDebug() ) {
        m_debug->show();
    }

//    connect( m_trayIcon, &QSystemTrayIcon::messageClicked, this, &SysTrayX::messageClicked);
    connect( m_tray_icon, &QSystemTrayIcon::activated, this, &SysTrayX::iconActivated);

    /*
     *  Connect debug link signals
     */
    connect( m_link, &SysTrayXLink::signalReceivedMessageLength, m_debug, &DebugWidget::slotReceivedMessageLength );
    connect( m_link, &SysTrayXLink::signalReceivedMessage, m_debug, &DebugWidget::slotReceivedMessage );

    connect( m_link, &SysTrayXLink::signalUnreadMail, m_debug, &DebugWidget::slotUnreadMail );

    connect( m_link, &SysTrayXLink::signalLinkReceiveError, m_debug, &DebugWidget::slotReceiveError );

    connect( m_debug, &DebugWidget::signalWriteMessage, m_link, &SysTrayXLink::slotLinkWrite );

    connect( m_pref_dialog, &PreferencesDialog::signalDebugMessage, m_debug, &DebugWidget::slotDebugMessage );
    connect( m_tray_icon, &SysTrayXIcon::signalDebugMessage, m_debug, &DebugWidget::slotDebugMessage );
    connect( m_link, &SysTrayXLink::signalDebugMessage, m_debug, &DebugWidget::slotDebugMessage );


    /*
     * Connect preferences signals
     */
    connect( m_preferences, &Preferences::signalIconTypeChange, m_tray_icon, &SysTrayXIcon::slotIconTypeChange );
    connect( m_preferences, &Preferences::signalIconDataChange, m_tray_icon, &SysTrayXIcon::slotIconDataChange );

    connect( m_preferences, &Preferences::signalIconTypeChange, m_pref_dialog, &PreferencesDialog::slotIconTypeChange );
    connect( m_preferences, &Preferences::signalIconDataChange, m_pref_dialog, &PreferencesDialog::slotIconDataChange );
    connect( m_preferences, &Preferences::signalDebugChange, m_pref_dialog, &PreferencesDialog::slotDebugChange );

    connect( m_preferences, &Preferences::signalIconTypeChange, m_link, &SysTrayXLink::slotIconTypeChange );
    connect( m_preferences, &Preferences::signalIconDataChange, m_link, &SysTrayXLink::slotIconDataChange );
    connect( m_preferences, &Preferences::signalDebugChange, m_link, &SysTrayXLink::slotDebugChange );

    connect( m_preferences, &Preferences::signalDebugChange, m_debug, &DebugWidget::slotDebugChange );

    /*
     * Connect link signals
     */
    connect( m_link, &SysTrayXLink::signalUnreadMail, m_tray_icon, &SysTrayXIcon::slotSetUnreadMail );
    connect( m_link, &SysTrayXLink::signalShutdown, this, &SysTrayX::slotShutdown );

    /*
     *  Request preferences from add-on
     */
    getPreferences();
}


/*
 *  Send a preferences request
 */
void SysTrayX::getPreferences()
{
    /*
     *  Request preferences from add-on
     */
    QByteArray request = QString( SysTrayX::JSON_PREF_REQUEST ).toUtf8();
    emit signalWriteMessage( request );
}


/*
 *  Handle a click on the system tray icon
 */
void SysTrayX::iconActivated( QSystemTrayIcon::ActivationReason reason )
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::MiddleClick:
        break;
    default:
        ;
    }
}


/*
 *  Create the actions for the system tray icon menu
 */
void SysTrayX::createActions()
{
/*
    m_minimizeAction = new QAction(tr("Mi&nimize"), this);
    connect( m_minimizeAction, &QAction::triggered, this, &QWidget::hide );

    m_maximizeAction = new QAction(tr("Ma&ximize"), this);
    connect( m_maximizeAction, &QAction::triggered, this, &QWidget::showMaximized );

    m_restoreAction = new QAction(tr("&Restore"), this);
    connect( m_restoreAction, &QAction::triggered, this, &QWidget::showNormal );
*/

    m_pref_action = new QAction(tr("&Preferences"), this);
    connect( m_pref_action, &QAction::triggered, m_pref_dialog, &PreferencesDialog::showNormal );

    m_quit_action = new QAction(tr("&Quit"), this);
    connect( m_quit_action, &QAction::triggered, qApp, &QCoreApplication::quit );

}


/*
 *  Create the system tray icon
 */
void SysTrayX::createTrayIcon()
{
    /*
     *  Setup menu actions
     */
    createActions();

    /*
     *  Setup menu
     */
    m_tray_icon_menu = new QMenu();
//    m_trayIconMenu->addAction( m_minimizeAction );
//    m_trayIconMenu->addAction( m_maximizeAction );
//    m_trayIconMenu->addAction( m_restoreAction );

    m_tray_icon_menu->addAction( m_pref_action );
    m_tray_icon_menu->addSeparator();
    m_tray_icon_menu->addAction( m_quit_action );

    /*
     *  Create system tray icon
     */
    m_tray_icon = new SysTrayXIcon( m_link, m_preferences );
    m_tray_icon->setContextMenu( m_tray_icon_menu );

    /*
     *  Set icon
     */
    m_tray_icon->setIconMime( m_preferences->getIconMime() );
    m_tray_icon->setIconData( m_preferences->getIconData() );
    m_tray_icon->setIconType( m_preferences->getIconType() );
}


/*
 *  Quit the app
 */
void SysTrayX::slotShutdown()
{
    /*
     *  Let's quit
     */
    QCoreApplication::quit();
}
