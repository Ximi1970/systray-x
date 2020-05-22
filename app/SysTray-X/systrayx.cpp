#include "systrayx.h"

/*
 *	Local includes
 */
#include "debugwidget.h"
#include "preferencesdialog.h"
#include "systrayxlink.h"
#include "systrayxicon.h"
#include "windowctrl.h"

/*
 *	Qt includes
 */
#include <QCoreApplication>
#include <QMenu>
#include <QStyle>
#include <QIcon>

/*
 *  Constants
 */
const QString   SysTrayX::JSON_PREF_REQUEST = "{\"preferences\":{}}";


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
     *  Setup window control
     */
    m_win_ctrl = new WindowCtrl( m_preferences );

#ifdef QT_NO_DEBUG

    if( !m_win_ctrl->thunderbirdStart() )
    {
        /*
         *  Let's quit
         */
        exit(0);
    }

#endif

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

    /*
     *  Connect debug link signals
     */
    connect( m_link, &SysTrayXLink::signalUnreadMail, m_debug, &DebugWidget::slotUnreadMail );

    connect( this, &SysTrayX::signalConsole, m_debug, &DebugWidget::slotConsole );
    connect( m_preferences, &Preferences::signalConsole, m_debug, &DebugWidget::slotConsole );
    connect( m_pref_dialog, &PreferencesDialog::signalConsole, m_debug, &DebugWidget::slotConsole );
    connect( m_link, &SysTrayXLink::signalConsole, m_debug, &DebugWidget::slotConsole );
    connect( m_win_ctrl, &WindowCtrl::signalConsole, m_debug, &DebugWidget::slotConsole );

    connect( m_debug, &DebugWidget::signalTest1ButtonClicked, m_win_ctrl, &WindowCtrl::slotWindowTest1 );
    connect( m_debug, &DebugWidget::signalTest2ButtonClicked, m_win_ctrl, &WindowCtrl::slotWindowTest2 );
    connect( m_debug, &DebugWidget::signalTest3ButtonClicked, m_win_ctrl, &WindowCtrl::slotWindowTest3 );

    /*
     *  Connect preferences signals
     */
    connect( m_preferences, &Preferences::signalDefaultIconTypeChange, m_tray_icon, &SysTrayXIcon::slotDefaultIconTypeChange );
    connect( m_preferences, &Preferences::signalDefaultIconDataChange, m_tray_icon, &SysTrayXIcon::slotDefaultIconDataChange );
    connect( m_preferences, &Preferences::signalIconTypeChange, m_tray_icon, &SysTrayXIcon::slotIconTypeChange );
    connect( m_preferences, &Preferences::signalIconDataChange, m_tray_icon, &SysTrayXIcon::slotIconDataChange );
    connect( m_preferences, &Preferences::signalShowNumberChange, m_tray_icon, &SysTrayXIcon::slotShowNumberChange );
    connect( m_preferences, &Preferences::signalNumberColorChange, m_tray_icon, &SysTrayXIcon::slotNumberColorChange );

    connect( m_preferences, &Preferences::signalMinimizeTypeChange, m_win_ctrl, &WindowCtrl::slotMinimizeTypeChange );
    connect( m_preferences, &Preferences::signalStartMinimizedChange, m_win_ctrl, &WindowCtrl::slotStartMinimizedChange );

    connect( m_preferences, &Preferences::signalDefaultIconTypeChange, m_pref_dialog, &PreferencesDialog::slotDefaultIconTypeChange );
    connect( m_preferences, &Preferences::signalDefaultIconDataChange, m_pref_dialog, &PreferencesDialog::slotDefaultIconDataChange );
    connect( m_preferences, &Preferences::signalIconTypeChange, m_pref_dialog, &PreferencesDialog::slotIconTypeChange );
    connect( m_preferences, &Preferences::signalIconDataChange, m_pref_dialog, &PreferencesDialog::slotIconDataChange );
    connect( m_preferences, &Preferences::signalShowNumberChange, m_pref_dialog, &PreferencesDialog::slotShowNumberChange );
    connect( m_preferences, &Preferences::signalNumberColorChange, m_pref_dialog, &PreferencesDialog::slotNumberColorChange );
    connect( m_preferences, &Preferences::signalCountTypeChange, m_pref_dialog, &PreferencesDialog::slotCountTypeChange );
    connect( m_preferences, &Preferences::signalMinimizeTypeChange, m_pref_dialog, &PreferencesDialog::slotMinimizeTypeChange );
    connect( m_preferences, &Preferences::signalStartMinimizedChange, m_pref_dialog, &PreferencesDialog::slotStartMinimizedChange );
    connect( m_preferences, &Preferences::signalMinimizeOnCloseChange, m_pref_dialog, &PreferencesDialog::slotMinimizeOnCloseChange );
    connect( m_preferences, &Preferences::signalDebugChange, m_pref_dialog, &PreferencesDialog::slotDebugChange );

    connect( m_preferences, &Preferences::signalDefaultIconTypeChange, m_link, &SysTrayXLink::slotDefaultIconTypeChange );
    connect( m_preferences, &Preferences::signalDefaultIconDataChange, m_link, &SysTrayXLink::slotDefaultIconDataChange );
    connect( m_preferences, &Preferences::signalIconTypeChange, m_link, &SysTrayXLink::slotIconTypeChange );
    connect( m_preferences, &Preferences::signalIconDataChange, m_link, &SysTrayXLink::slotIconDataChange );
    connect( m_preferences, &Preferences::signalShowNumberChange, m_link, &SysTrayXLink::slotShowNumberChange );
    connect( m_preferences, &Preferences::signalNumberColorChange, m_link, &SysTrayXLink::slotNumberColorChange );
    connect( m_preferences, &Preferences::signalCountTypeChange, m_link, &SysTrayXLink::slotCountTypeChange );
    connect( m_preferences, &Preferences::signalMinimizeTypeChange, m_link, &SysTrayXLink::slotMinimizeTypeChange );
    connect( m_preferences, &Preferences::signalStartMinimizedChange, m_link, &SysTrayXLink::slotStartMinimizedChange );
    connect( m_preferences, &Preferences::signalMinimizeOnCloseChange, m_link, &SysTrayXLink::slotMinimizeOnCloseChange );
    connect( m_preferences, &Preferences::signalDebugChange, m_link, &SysTrayXLink::slotDebugChange );

    connect( m_preferences, &Preferences::signalDebugChange, m_debug, &DebugWidget::slotDebugChange );

    /*
     *  Connect link signals
     */
    connect( m_link, &SysTrayXLink::signalUnreadMail, m_tray_icon, &SysTrayXIcon::slotSetUnreadMail );
    connect( m_link, &SysTrayXLink::signalAddOnShutdown, this, &SysTrayX::slotAddOnShutdown );
    connect( m_link, &SysTrayXLink::signalWindowState, m_win_ctrl, &WindowCtrl::slotWindowState );
    connect( m_link, &SysTrayXLink::signalTitle, m_win_ctrl, &WindowCtrl::slotWindowTitle );
    connect( m_link, &SysTrayXLink::signalVersion, this, &SysTrayX::slotVersion );

    /*
     *  Connect system tray signals
     */
    connect( m_tray_icon, &SysTrayXIcon::signalShowHide, m_win_ctrl, &WindowCtrl::slotShowHide );

    /*
     *  SysTrayX
     */
    connect( this, &SysTrayX::signalClose, m_win_ctrl, &WindowCtrl::slotClose );

    /*
     *  Request preferences from add-on
     */
    getPreferences();
}


/*
 *  Send a preferences request
 */
void    SysTrayX::getPreferences()
{
    /*
     *  Request preferences from add-on
     */
    QByteArray request = QString( SysTrayX::JSON_PREF_REQUEST ).toUtf8();
    emit signalWriteMessage( request );
}


/*
 *  Create the actions for the system tray icon menu
 */
void    SysTrayX::createActions()
{
    m_showhide_action = new QAction(tr("&Show/Hide"), this);
    m_showhide_action->setIcon( QIcon( ":/files/icons/window-restore.png" ) );
    connect( m_showhide_action, &QAction::triggered, m_win_ctrl, &WindowCtrl::slotShowHide );

    m_pref_action = new QAction(tr("&Preferences"), this);
    m_pref_action->setIcon( QIcon( ":/files/icons/gtk-preferences.png" ) );
    connect( m_pref_action, &QAction::triggered, m_pref_dialog, &PreferencesDialog::showNormal );

    m_about_action = new QAction(tr("&About"), this);
    m_about_action->setIcon( QIcon( ":/files/icons/help-about.png" ) );
    connect( m_about_action, &QAction::triggered, this, &SysTrayX::slotAbout );

    m_quit_action = new QAction( tr("&Quit"), this );
    m_quit_action->setIcon( QIcon( ":/files/icons/window-close.png" ) );
    connect( m_quit_action, &QAction::triggered, this, &SysTrayX::slotShutdown );
}


/*
 *  Create the system tray icon
 */
void    SysTrayX::createTrayIcon()
{
    /*
     *  Setup menu actions
     */
    createActions();

    /*
     *  Setup menu
     */
    m_tray_icon_menu = new QMenu();

    m_tray_icon_menu->addAction( m_showhide_action );
    m_tray_icon_menu->addSeparator();
    m_tray_icon_menu->addAction( m_pref_action );
    m_tray_icon_menu->addAction( m_about_action );
    m_tray_icon_menu->addSeparator();
    m_tray_icon_menu->addAction( m_quit_action );

    /*
     *  Create system tray icon
     */
    m_tray_icon = new SysTrayXIcon( m_link, m_preferences );
    m_tray_icon->setContextMenu( m_tray_icon_menu );

    /*
     *  Set default icon
     */
    m_tray_icon->setDefaultIconMime( m_preferences->getDefaultIconMime() );
    m_tray_icon->setDefaultIconData( m_preferences->getDefaultIconData() );
    m_tray_icon->setDefaultIconType( m_preferences->getDefaultIconType() );

    /*
     *  Set icon
     */
    m_tray_icon->setIconMime( m_preferences->getIconMime() );
    m_tray_icon->setIconData( m_preferences->getIconData() );
    m_tray_icon->setIconType( m_preferences->getIconType() );
}


/*
 *  Quit the app by add-on request
 */
void    SysTrayX::slotAddOnShutdown()
{
    /*
     *  Hide systray icon to prevent ghost systray icon in Windows
     */
    m_tray_icon->hide();

    /*
     *  Close the TB window
     */
    emit signalClose();

    /*
     *  Let's quit
     */
    QCoreApplication::quit();
}


/*
 *  Quit the app by quit menu
 */
void    SysTrayX::slotShutdown()
{

    if( m_preferences->getMinimizeOnClose() )
    {
        m_link->sendShutdown();
    }
    else
    {
        /*
         *  Hide systray icon to prevent ghost systray icon in Windows
         */
        m_tray_icon->hide();

        /*
         *  Close the TB window
         */
        emit signalClose();

        /*
         *  Let's quit
         */
        QCoreApplication::quit();
    }
}


/*
 *  Show the about dialog
 */
void    SysTrayX::slotAbout()
{
    Ui::AboutDialog ui;
    QDialog dialog;

    ui.setupUi( &dialog );

    ui.version->setText( m_preferences->getVersion() );
    ui.build->setText( m_preferences->getBuild() );
    ui.hash->setText( m_preferences->getHash() );
    ui.branch->setText( m_preferences->getBranch() );

    dialog.exec();
}


/*
 *  Handle the version info from the addon
 */
void    SysTrayX::slotVersion( QString version )
{
    if( version != m_preferences->getVersion() )
    {
        m_tray_icon->showMessage("SysTray-X Warning", "Version mismatch addon and app",
            QSystemTrayIcon::Warning );
    }
}
