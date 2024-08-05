#include "systrayx.h"

/*
 *	Local includes
 */
#include "debugwidget.h"
#include "preferencesdialog.h"
#include "systrayxlink.h"
#include "systrayxicon.h"
#include "systrayxstatusnotifier.h"
#include "windowctrl.h"
#include "shortcut.h"

/*
 *	Qt includes
 */
#include <QCoreApplication>
#include <QProcess>
#include <QMenu>
#include <QIcon>
#include <QTimer>


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
     *  Initialize
     */
    m_tray_icon = nullptr;
    m_kde_tray_icon = nullptr;

    m_tray_icon_menu = nullptr;

    m_unread_mail = 0;
    m_new_mail = 0;
    m_locale = QString();

    m_show_hide_shortcut = nullptr;

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
     *  Setup debug window
     */
    m_debug = new DebugWidget( m_preferences );
    if( m_preferences->getDebug() ) {
        m_debug->show();
    }

    /*
     *  Connect debug link signals
     */
    connect( m_link, &SysTrayXLink::signalMailCount, m_debug, &DebugWidget::slotMailCount );

    connect( this, &SysTrayX::signalConsole, m_debug, &DebugWidget::slotConsole );
    connect( m_preferences, &Preferences::signalConsole, m_debug, &DebugWidget::slotConsole );
    connect( m_pref_dialog, &PreferencesDialog::signalConsole, m_debug, &DebugWidget::slotConsole );
    connect( m_link, &SysTrayXLink::signalConsole, m_debug, &DebugWidget::slotConsole );
    connect( m_win_ctrl, &WindowCtrl::signalConsole, m_debug, &DebugWidget::slotConsole );

    connect( m_debug, &DebugWidget::signalTest1ButtonClicked, m_win_ctrl, &WindowCtrl::slotWindowTest1 );
    connect( m_debug, &DebugWidget::signalTest2ButtonClicked, m_win_ctrl, &WindowCtrl::slotWindowTest2 );
    connect( m_debug, &DebugWidget::signalTest3ButtonClicked, m_win_ctrl, &WindowCtrl::slotWindowTest3 );
    connect( m_debug, &DebugWidget::signalTest4ButtonClicked, m_win_ctrl, &WindowCtrl::slotWindowTest4 );

#ifdef Q_OS_UNIX

    // connect( m_win_ctrl, &WindowCtrl::signalHideDefaultIconChange, this, &SysTrayX::slotSelectIconObject );

#endif

    /*
     *  Connect preferences signals
     */
    connect( m_preferences, &Preferences::signalMinimizeTypeChange, m_win_ctrl, &WindowCtrl::slotMinimizeTypeChange );
    connect( m_preferences, &Preferences::signalMinimizeIconTypeChange, m_win_ctrl, &WindowCtrl::slotMinimizeIconTypeChange );
    connect( m_preferences, &Preferences::signalCloseTypeChange, m_win_ctrl, &WindowCtrl::slotCloseTypeChange );

    connect( m_preferences, &Preferences::signalDefaultIconTypeChange, m_pref_dialog, &PreferencesDialog::slotDefaultIconTypeChange );
    connect( m_preferences, &Preferences::signalDefaultIconDataChange, m_pref_dialog, &PreferencesDialog::slotDefaultIconDataChange );
    connect( m_preferences, &Preferences::signalHideDefaultIconChange, m_pref_dialog, &PreferencesDialog::slotHideDefaultIconChange );
    connect( m_preferences, &Preferences::signalIconTypeChange, m_pref_dialog, &PreferencesDialog::slotIconTypeChange );
    connect( m_preferences, &Preferences::signalIconDataChange, m_pref_dialog, &PreferencesDialog::slotIconDataChange );
    connect( m_preferences, &Preferences::signalMinimizeTypeChange, m_pref_dialog, &PreferencesDialog::slotMinimizeTypeChange );
    connect( m_preferences, &Preferences::signalMinimizeIconTypeChange, m_pref_dialog, &PreferencesDialog::slotMinimizeIconTypeChange );
    connect( m_preferences, &Preferences::signalWindowPositionsCorrectionChange, m_pref_dialog, &PreferencesDialog::slotWindowPositionsCorrectionChange );
    connect( m_preferences, &Preferences::signalWindowPositionsCorrectionTypeChange, m_pref_dialog, &PreferencesDialog::slotWindowPositionsCorrectionTypeChange );
    connect( m_preferences, &Preferences::signalStartupTypeChange, m_pref_dialog, &PreferencesDialog::slotStartupTypeChange );
    connect( m_preferences, &Preferences::signalRestoreWindowPositionsChange, m_pref_dialog, &PreferencesDialog::slotRestoreWindowPositionsChange );
    connect( m_preferences, &Preferences::signalCloseTypeChange, m_pref_dialog, &PreferencesDialog::slotCloseTypeChange );
    connect( m_preferences, &Preferences::signalInvertIconChange, m_pref_dialog, &PreferencesDialog::slotInvertIconChange );
    connect( m_preferences, &Preferences::signalShowNumberChange, m_pref_dialog, &PreferencesDialog::slotShowNumberChange );
    connect( m_preferences, &Preferences::signalShowNewIndicatorChange, m_pref_dialog, &PreferencesDialog::slotShowNewIndicatorChange );
    connect( m_preferences, &Preferences::signalStartupDelayChange, m_pref_dialog, &PreferencesDialog::slotStartupDelayChange );
    connect( m_preferences, &Preferences::signalApiCountMethodChange, m_pref_dialog, &PreferencesDialog::slotApiCountMethodChange );
    connect( m_preferences, &Preferences::signalCountTypeChange, m_pref_dialog, &PreferencesDialog::slotCountTypeChange );
    connect( m_preferences, &Preferences::signalNumberColorChange, m_pref_dialog, &PreferencesDialog::slotNumberColorChange );
    connect( m_preferences, &Preferences::signalNumberSizeChange, m_pref_dialog, &PreferencesDialog::slotNumberSizeChange );
    connect( m_preferences, &Preferences::signalNumberAlignmentChange, m_pref_dialog, &PreferencesDialog::slotNumberAlignmentChange );
    connect( m_preferences, &Preferences::signalNumberMarginsChange, m_pref_dialog, &PreferencesDialog::slotNumberMarginsChange );
    connect( m_preferences, &Preferences::signalNewIndicatorTypeChange, m_pref_dialog, &PreferencesDialog::slotNewIndicatorTypeChange );
    connect( m_preferences, &Preferences::signalNewShadeColorChange, m_pref_dialog, &PreferencesDialog::slotNewShadeColorChange );
    connect( m_preferences, &Preferences::signalStartAppChange, m_pref_dialog, &PreferencesDialog::slotStartAppChange );
    connect( m_preferences, &Preferences::signalStartAppArgsChange, m_pref_dialog, &PreferencesDialog::slotStartAppArgsChange );
    connect( m_preferences, &Preferences::signalCloseAppChange, m_pref_dialog, &PreferencesDialog::slotCloseAppChange );
    connect( m_preferences, &Preferences::signalCloseAppArgsChange, m_pref_dialog, &PreferencesDialog::slotCloseAppArgsChange );
    connect( m_preferences, &Preferences::signalShowHideShortcutChange, m_pref_dialog, &PreferencesDialog::slotShowHideShortcutChange );
    connect( m_preferences, &Preferences::signalDebugChange, m_pref_dialog, &PreferencesDialog::slotDebugChange );

    connect( m_pref_dialog, &PreferencesDialog::signalPreferencesChanged, m_link, &SysTrayXLink::slotPreferencesChanged );

    connect( m_preferences, &Preferences::signalHideDefaultIconChange, this,  &SysTrayX::slotSelectIconObjectPref );

    connect( m_preferences, &Preferences::signalDebugChange, m_debug, &DebugWidget::slotDebugChange );

#if defined( SHORTCUTS )

    if( m_preferences->getShortcutsOption() )
    {
        connect( m_preferences, &Preferences::signalShowHideShortcutChange, this, &SysTrayX::slotShowHideShortcutChange );
    }

#endif

    /*
     *  Connect link signals
     */
    connect( m_link, &SysTrayXLink::signalAddOnShutdown, this, &SysTrayX::slotAddOnShutdown );
    connect( m_link, &SysTrayXLink::signalErrorAddOnShutdown, this, &SysTrayX::slotErrorAddOnShutdown );
    connect( m_link, &SysTrayXLink::signalWindowState, m_win_ctrl, &WindowCtrl::slotWindowState );
    connect( m_link, &SysTrayXLink::signalNewWindow, m_win_ctrl, &WindowCtrl::slotNewWindow );
    connect( m_link, &SysTrayXLink::signalCloseWindow, m_win_ctrl, &WindowCtrl::slotCloseWindow );
    connect( m_link, &SysTrayXLink::signalMailCount, this, &SysTrayX::slotMailCount );
    connect( m_link, &SysTrayXLink::signalVersion, this, &SysTrayX::slotVersion );
    connect( m_link, &SysTrayXLink::signalKdeIntegration, this, &SysTrayX::slotSelectIconObject );
    connect( m_link, &SysTrayXLink::signalLocale, this, &SysTrayX::slotLoadLanguage );
    connect( m_link, &SysTrayXLink::signalStartApp, this, &SysTrayX::slotStartApp );
    connect( m_link, &SysTrayXLink::signalCloseApp, this, &SysTrayX::slotCloseApp );

#ifdef Q_OS_UNIX

    connect( m_link, &SysTrayXLink::signalWindowPositions, m_win_ctrl, &WindowCtrl::slotPositions );

    connect( m_win_ctrl, &WindowCtrl::signalPositions, m_link, &SysTrayXLink::slotPositions );

#endif

    /*
     *  SysTrayX
     */
    connect( this, &SysTrayX::signalClose, m_win_ctrl, &WindowCtrl::slotClose );

    /*
     *  Request preferences from add-on
     */
    getPreferences();

    m_preferences->displayDebug();

/*
    m_preferences->setBrowserVersion( "115.1.0" );
//    m_preferences->setBrowserVersion( "102.2.3" );

    if( m_preferences->getShortcutsOption() )
    {
        m_preferences->setShowHideShortcut( QKeySequence( Qt::CTRL | Qt::Key_P ) );
    }

    slotLoadLanguage( "en-US" );
    //slotLoadLanguage( "it" );
    //slotLoadLanguage( "nl" );
    //slotLoadLanguage( "pt-BR" );
    //slotLoadLanguage( "ru" );
    slotSelectIconObject( true );

    slotMailCount( 10, 1 );

//    m_preferences->setStartApp( "/home/maxime/test.sh" );
//    m_preferences->setStartAppArgs( "/home/maxime/startup.txt StartupString" );
//    slotStartApp();
*/
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
 *  Create the menu for the system tray icon
 */
void    SysTrayX::createMenu()
{
    m_showhide_action = new QAction(tr("&Show/Hide"), this);
    m_showhide_action->setIcon( QIcon( ":/files/icons/window-restore.png" ) );
    connect( m_showhide_action, &QAction::triggered, m_win_ctrl, &WindowCtrl::slotShowHide );

    m_pref_action = new QAction(tr("&Preferences"), this);
    m_pref_action->setIcon( QIcon( ":/files/icons/gtk-preferences.png" ) );
    connect( m_pref_action, &QAction::triggered, m_pref_dialog, &PreferencesDialog::slotShowDialog );

    m_about_action = new QAction(tr("&About"), this);
    m_about_action->setIcon( QIcon( ":/files/icons/help-about.png" ) );
    connect( m_about_action, &QAction::triggered, this, &SysTrayX::slotAbout );

    m_quit_action = new QAction( tr("&Quit"), this );
    m_quit_action->setIcon( QIcon( ":/files/icons/window-close.png" ) );
    connect( m_quit_action, &QAction::triggered, this, &SysTrayX::slotShutdown );

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
}


/*
 *  Destroy the icon menu
 */
void    SysTrayX::destroyMenu()
{
    if( m_tray_icon_menu )
    {
        delete m_tray_icon_menu;
        m_tray_icon_menu = nullptr;
    }
}


/*
 *  Show / create tray icon
 */
void    SysTrayX::showTrayIcon()
{
    if( !m_tray_icon )
    {        
        /*
         *  Setup tray menu
         */
        createMenu();

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

        /*
         *  Connect the world
         */
        connect( m_tray_icon, &SysTrayXIcon::signalShowHide, m_win_ctrl, &WindowCtrl::slotShowHide );

        connect( m_preferences, &Preferences::signalDefaultIconTypeChange, m_tray_icon, &SysTrayXIcon::slotDefaultIconTypeChange );
        connect( m_preferences, &Preferences::signalDefaultIconDataChange, m_tray_icon, &SysTrayXIcon::slotDefaultIconDataChange );
        connect( m_preferences, &Preferences::signalIconTypeChange, m_tray_icon, &SysTrayXIcon::slotIconTypeChange );
        connect( m_preferences, &Preferences::signalIconDataChange, m_tray_icon, &SysTrayXIcon::slotIconDataChange );
        connect( m_preferences, &Preferences::signalInvertIconChange, m_tray_icon, &SysTrayXIcon::slotInvertIconChange );
        connect( m_preferences, &Preferences::signalShowNumberChange, m_tray_icon, &SysTrayXIcon::slotShowNumberChange );
        connect( m_preferences, &Preferences::signalShowNewIndicatorChange, m_tray_icon, &SysTrayXIcon::slotShowNewIndicatorChange );
        connect( m_preferences, &Preferences::signalNumberColorChange, m_tray_icon, &SysTrayXIcon::slotNumberColorChange );
        connect( m_preferences, &Preferences::signalNumberSizeChange, m_tray_icon, &SysTrayXIcon::slotNumberSizeChange );
        connect( m_preferences, &Preferences::signalNumberAlignmentChange, m_tray_icon, &SysTrayXIcon::slotNumberAlignmentChange );
        connect( m_preferences, &Preferences::signalNumberMarginsChange, m_tray_icon, &SysTrayXIcon::slotNumberMarginsChange );
        connect( m_preferences, &Preferences::signalNewIndicatorTypeChange, m_tray_icon, &SysTrayXIcon::slotNewIndicatorTypeChange );
        connect( m_preferences, &Preferences::signalNewShadeColorChange, m_tray_icon, &SysTrayXIcon::slotNewShadeColorChange );

        connect( m_link, &SysTrayXLink::signalMailCount, m_tray_icon, &SysTrayXIcon::slotMailCount );

        connect( this, &SysTrayX::signalMailCount, m_tray_icon, &SysTrayXIcon::slotMailCount );

        /*
         *  Show it
         */
        m_tray_icon->show();

        QTimer::singleShot(500, this, &SysTrayX::resendMailCount);
    }
}


/*
 *  Hide / remove tray icon
 */
void    SysTrayX::hideTrayIcon()
{
    if( m_tray_icon )
    {
        /*
         *  Disconnect all signals
         */
        disconnect( m_tray_icon, &SysTrayXIcon::signalShowHide, m_win_ctrl, &WindowCtrl::slotShowHide );

        disconnect( m_preferences, &Preferences::signalDefaultIconTypeChange, m_tray_icon, &SysTrayXIcon::slotDefaultIconTypeChange );
        disconnect( m_preferences, &Preferences::signalDefaultIconDataChange, m_tray_icon, &SysTrayXIcon::slotDefaultIconDataChange );
        disconnect( m_preferences, &Preferences::signalIconTypeChange, m_tray_icon, &SysTrayXIcon::slotIconTypeChange );
        disconnect( m_preferences, &Preferences::signalIconDataChange, m_tray_icon, &SysTrayXIcon::slotIconDataChange );
        disconnect( m_preferences, &Preferences::signalInvertIconChange, m_tray_icon, &SysTrayXIcon::slotInvertIconChange );
        disconnect( m_preferences, &Preferences::signalShowNumberChange, m_tray_icon, &SysTrayXIcon::slotShowNumberChange );
        disconnect( m_preferences, &Preferences::signalShowNewIndicatorChange, m_tray_icon, &SysTrayXIcon::slotShowNewIndicatorChange );
        disconnect( m_preferences, &Preferences::signalNumberColorChange, m_tray_icon, &SysTrayXIcon::slotNumberColorChange );
        disconnect( m_preferences, &Preferences::signalNumberSizeChange, m_tray_icon, &SysTrayXIcon::slotNumberSizeChange );
        disconnect( m_preferences, &Preferences::signalNumberAlignmentChange, m_tray_icon, &SysTrayXIcon::slotNumberAlignmentChange );
        disconnect( m_preferences, &Preferences::signalNumberMarginsChange, m_tray_icon, &SysTrayXIcon::slotNumberMarginsChange );
        disconnect( m_preferences, &Preferences::signalNewIndicatorTypeChange, m_tray_icon, &SysTrayXIcon::slotNewIndicatorTypeChange );
        disconnect( m_preferences, &Preferences::signalNewShadeColorChange, m_tray_icon, &SysTrayXIcon::slotNewShadeColorChange );

        disconnect( m_link, &SysTrayXLink::signalMailCount, m_tray_icon, &SysTrayXIcon::slotMailCount );

        disconnect( this, &SysTrayX::signalMailCount, m_tray_icon, &SysTrayXIcon::slotMailCount );

        /*
         *  Hide the icon  first to prevent "ghosts"
         */
//        m_tray_icon->hide();

        /*
         *  Remove the icon
         */
        delete m_tray_icon;
        m_tray_icon = nullptr;

        /*
         *  Destroy the mennu
         */
        destroyMenu();
    }
}


#if defined( Q_OS_UNIX ) && defined( KDE_INTEGRATION )

/*
 *  Show / create tray icon
 */
void    SysTrayX::showKdeTrayIcon()
{
    if( !m_kde_tray_icon )
    {
        /*
         *  Setup tray menu
         */
        createMenu();

        /*
         *  Create system tray icon
         */
        m_kde_tray_icon = new SysTrayXStatusNotifier( m_link, m_preferences );
        m_kde_tray_icon->setStandardActionsEnabled( false );
        m_kde_tray_icon->setContextMenu( m_tray_icon_menu );

        /*
         *  Set default icon
         */
        m_kde_tray_icon->setDefaultIconMime( m_preferences->getDefaultIconMime() );
        m_kde_tray_icon->setDefaultIconData( m_preferences->getDefaultIconData() );
        m_kde_tray_icon->setDefaultIconType( m_preferences->getDefaultIconType() );

        /*
         *  Set icon
         */
        m_kde_tray_icon->setIconMime( m_preferences->getIconMime() );
        m_kde_tray_icon->setIconData( m_preferences->getIconData() );
        m_kde_tray_icon->setIconType( m_preferences->getIconType() );

        /*
         *  Connect the world
         */
        connect( m_kde_tray_icon, &SysTrayXStatusNotifier::signalShowHide, m_win_ctrl, &WindowCtrl::slotShowHide );

        connect( m_preferences, &Preferences::signalDefaultIconTypeChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotDefaultIconTypeChange );
        connect( m_preferences, &Preferences::signalDefaultIconDataChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotDefaultIconDataChange );
        connect( m_preferences, &Preferences::signalHideDefaultIconChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotHideDefaultIconChange );
        connect( m_preferences, &Preferences::signalIconTypeChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotIconTypeChange );
        connect( m_preferences, &Preferences::signalIconDataChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotIconDataChange );
        connect( m_preferences, &Preferences::signalInvertIconChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotInvertIconChange );
        connect( m_preferences, &Preferences::signalShowNumberChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotShowNumberChange );
        connect( m_preferences, &Preferences::signalShowNewIndicatorChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotShowNewIndicatorChange );
        connect( m_preferences, &Preferences::signalNumberColorChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNumberColorChange );
        connect( m_preferences, &Preferences::signalNumberSizeChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNumberSizeChange );
        connect( m_preferences, &Preferences::signalNumberAlignmentChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNumberAlignmentChange );
        connect( m_preferences, &Preferences::signalNumberMarginsChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNumberMarginsChange );
        connect( m_preferences, &Preferences::signalNewIndicatorTypeChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNewIndicatorTypeChange );
        connect( m_preferences, &Preferences::signalNewShadeColorChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNewShadeColorChange );

        connect( m_link, &SysTrayXLink::signalMailCount, m_kde_tray_icon, &SysTrayXStatusNotifier::slotMailCount );

        connect( this, &SysTrayX::signalMailCount, m_kde_tray_icon, &SysTrayXStatusNotifier::slotMailCount );

        /*
         *  Show
         */
        QTimer::singleShot(500, this, &SysTrayX::resendMailCount);
    }
}


/*
 *  Hide / remove tray icon
 */
void    SysTrayX::hideKdeTrayIcon()
{
    if( m_kde_tray_icon )
    {
        /*
         *  Disconnect all signals
         */
        disconnect( m_kde_tray_icon, &SysTrayXStatusNotifier::signalShowHide, m_win_ctrl, &WindowCtrl::slotShowHide );

        disconnect( m_preferences, &Preferences::signalDefaultIconTypeChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotDefaultIconTypeChange );
        disconnect( m_preferences, &Preferences::signalDefaultIconDataChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotDefaultIconDataChange );
        disconnect( m_preferences, &Preferences::signalHideDefaultIconChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotHideDefaultIconChange );
        disconnect( m_preferences, &Preferences::signalIconTypeChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotIconTypeChange );
        disconnect( m_preferences, &Preferences::signalIconDataChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotIconDataChange );
        disconnect( m_preferences, &Preferences::signalInvertIconChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotInvertIconChange );
        disconnect( m_preferences, &Preferences::signalShowNumberChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotShowNumberChange );
        disconnect( m_preferences, &Preferences::signalShowNewIndicatorChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotShowNewIndicatorChange );
        disconnect( m_preferences, &Preferences::signalNumberColorChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNumberColorChange );
        disconnect( m_preferences, &Preferences::signalNumberSizeChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNumberSizeChange );
        disconnect( m_preferences, &Preferences::signalNumberAlignmentChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNumberAlignmentChange );
        disconnect( m_preferences, &Preferences::signalNumberMarginsChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNumberMarginsChange );
        disconnect( m_preferences, &Preferences::signalNewIndicatorTypeChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNewIndicatorTypeChange );
        disconnect( m_preferences, &Preferences::signalNewShadeColorChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNewShadeColorChange );

        disconnect( m_link, &SysTrayXLink::signalMailCount, m_kde_tray_icon, &SysTrayXStatusNotifier::slotMailCount );

        disconnect( this, &SysTrayX::signalMailCount, m_kde_tray_icon, &SysTrayXStatusNotifier::slotMailCount );

        /*
         *  Remove the notifier icon
         */
        delete m_kde_tray_icon;
        m_kde_tray_icon = nullptr;

        /*
         *  Destroy the mennu
         */
        m_tray_icon_menu = nullptr;
        //destroyMenu();
    }
}

#endif


/*
 *  Select the prefered icon
 */
void    SysTrayX::slotSelectIconObjectPref()
{
    slotSelectIconObject( m_preferences->getHideDefaultIcon() );
}

void    SysTrayX::slotSelectIconObject( bool state )
{

#if defined( Q_OS_UNIX ) && defined( KDE_INTEGRATION )

    if( state )
    {
        //  Use the KDE icon object
//        emit signalConsole("Enable KDE icon");

        //  Remove the Qt tray icon
        hideTrayIcon();

        //  Setup KDE tray icon
        showKdeTrayIcon();
    }
    else
    {
        //  Use default Qt system tray icon
//        emit signalConsole("Enable Qt icon");

        //  Remove KDE trsy icon
        hideKdeTrayIcon();

        //  Setup the Qt tray icon
        showTrayIcon();
    }

#else

    Q_UNUSED( state )

    //  Setup the Qt tray icon
    showTrayIcon();

#endif

}


/*
 *  Resend unread/new mail
 */
void    SysTrayX::resendMailCount()
{
    emit signalMailCount( m_unread_mail, m_new_mail );
}


/*
 *  Quit the app by add-on request
 */
void    SysTrayX::slotAddOnShutdown()
{
    /*
     *  Hide systray icon to prevent ghost systray icon in Windows
     */
    if( m_tray_icon )
    {
        m_tray_icon->hide();
    }

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
 *  Quit the app by add-on request
 */
void    SysTrayX::slotErrorAddOnShutdown()
{
    /*
     *  Hide systray icon to prevent ghost systray icon in Windows
     */
    if( m_tray_icon )
    {
        m_tray_icon->hide();
    }

    /*
     *  Launch close application
     */
    slotCloseApp();

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
    /*
     *  What shall we do?
     */
    switch( m_preferences->getCloseType() )
    {
        case Preferences::PREF_DEFAULT_CLOSE_WINDOWS:
        {
            /*
             *  Hide systray icon to prevent ghost systray icon in Windows
             */
            if( m_tray_icon )
            {
                m_tray_icon->hide();
            }

            /*
             *  Close the TB window
             */
            emit signalClose();

            /*
             *  Let's quit
             */
            QCoreApplication::quit();
            break;
        }

        case Preferences::PREF_MINIMIZE_MAIN_CLOSE_CHILDREN_WINDOWS:
        case Preferences::PREF_MINIMIZE_ALL_WINDOWS:
        case Preferences::PREF_MINIMIZE_MAIN_TRAY_CLOSE_CHILDREN_WINDOWS:
        case Preferences::PREF_MINIMIZE_ALL_WINDOWS_TRAY:
        {
            m_link->sendShutdown();
            break;
        }
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

    /*
     *  Set the translation for the button box
     */
    ui.buttonBox->button( QDialogButtonBox::Close )->setText( tr( "Close" ) );

    dialog.exec();
}


/*
 *  Handle the version info from the addon
 */
void    SysTrayX::slotVersion( QString version )
{
    if( version != m_preferences->getVersion() )
    {
        if( m_tray_icon )
        {
            m_tray_icon->showMessage("SysTray-X Warning", "Version mismatch addon and app",
                QSystemTrayIcon::Warning );
        }

#if defined( Q_OS_UNIX ) && defined( KDE_INTEGRATION )

        if( m_kde_tray_icon )
        {
            m_kde_tray_icon->showMessage("SysTray-X Warning", "Version mismatch addon and app", ":/files/icons/dialog-warning.png" );
        }

#endif

    }
}


/*
 *  Handle mail count signal
 */
void    SysTrayX::slotMailCount( int unread_mail, int new_mail )
{
    m_unread_mail = unread_mail;
    m_new_mail = new_mail;
}


/*
 *  Handle a change in locale
 */
void    SysTrayX::slotLoadLanguage( QString locale )
{
    if( m_locale != locale)
    {
        m_locale = locale;

        if (!m_translator.isEmpty())
        {
            qApp->removeTranslator( &m_translator );
        }

        QString locale_path = "SysTray-X."+ locale;
        (void)m_translator.load( locale_path, ":/languages/" );
//        bool status = m_translator.load( locale_path, ":/languages/" );
//        emit signalConsole( QString( "Language loaded %1").arg(status));
        qApp->installTranslator( &m_translator );
    }
}


/*
 *  Handle a startup app launch request
 */
void    SysTrayX::slotStartApp()
{
    QString app = m_preferences->getStartApp();
    QString args = m_preferences->getStartAppArgs();
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QStringList args_list = args.split( ' ', Qt::SkipEmptyParts );
#else
    QStringList args_list = args.split( ' ', QString::SkipEmptyParts );
#endif

    if( !app.isEmpty() )
    {
        QProcess::startDetached( app, args_list );
    }
}


/*
 *  Handle a close app launch request
 */
void    SysTrayX::slotCloseApp()
{
    QString app = m_preferences->getCloseApp();
    QString args = m_preferences->getCloseAppArgs();
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QStringList args_list = args.split( ' ', Qt::SkipEmptyParts );
#else
    QStringList args_list = args.split( ' ', QString::SkipEmptyParts );
#endif

    if( !app.isEmpty() )
    {
        QProcess::startDetached( app, args_list );
    }
}

void    SysTrayX::slotShowHideShortcutChange()
{
    if( m_show_hide_shortcut != nullptr )
    {
        disconnect( m_show_hide_shortcut, &Shortcut::activated, m_win_ctrl, &WindowCtrl::slotShowHide );

        delete m_show_hide_shortcut;
        m_show_hide_shortcut = nullptr;
    }

    m_show_hide_shortcut = new Shortcut( m_preferences->getShowHideShortcut(), this );
    connect( m_show_hide_shortcut, &Shortcut::activated, m_win_ctrl, &WindowCtrl::slotShowHide );
}
