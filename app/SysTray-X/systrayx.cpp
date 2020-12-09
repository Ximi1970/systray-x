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

/*
 *	Qt includes
 */
#include <QCoreApplication>
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
    m_locale = QString();

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
    connect( m_link, &SysTrayXLink::signalUnreadMail, m_debug, &DebugWidget::slotUnreadMail );

    connect( this, &SysTrayX::signalConsole, m_debug, &DebugWidget::slotConsole );
    connect( m_preferences, &Preferences::signalConsole, m_debug, &DebugWidget::slotConsole );
    connect( m_pref_dialog, &PreferencesDialog::signalConsole, m_debug, &DebugWidget::slotConsole );
    connect( m_link, &SysTrayXLink::signalConsole, m_debug, &DebugWidget::slotConsole );
    connect( m_win_ctrl, &WindowCtrl::signalConsole, m_debug, &DebugWidget::slotConsole );

    connect( m_debug, &DebugWidget::signalTest1ButtonClicked, m_win_ctrl, &WindowCtrl::slotWindowTest1 );
    connect( m_debug, &DebugWidget::signalTest2ButtonClicked, m_win_ctrl, &WindowCtrl::slotWindowTest2 );
    connect( m_debug, &DebugWidget::signalTest3ButtonClicked, m_win_ctrl, &WindowCtrl::slotWindowTest3 );

#ifdef Q_OS_UNIX

    // connect( m_win_ctrl, &WindowCtrl::signalHideDefaultIconChange, this, &SysTrayX::slotSelectIconObject );

#endif

    connect( this, &SysTrayX::signalConsole, m_debug, &DebugWidget::slotConsole );

    /*
     *  Connect preferences signals
     */
    connect( m_preferences, &Preferences::signalMinimizeTypeChange, m_win_ctrl, &WindowCtrl::slotMinimizeTypeChange );
    connect( m_preferences, &Preferences::signalStartMinimizedChange, m_win_ctrl, &WindowCtrl::slotStartMinimizedChange );

    connect( m_preferences, &Preferences::signalDefaultIconTypeChange, m_pref_dialog, &PreferencesDialog::slotDefaultIconTypeChange );
    connect( m_preferences, &Preferences::signalDefaultIconDataChange, m_pref_dialog, &PreferencesDialog::slotDefaultIconDataChange );
    connect( m_preferences, &Preferences::signalHideDefaultIconChange, m_pref_dialog, &PreferencesDialog::slotHideDefaultIconChange );
    connect( m_preferences, &Preferences::signalIconTypeChange, m_pref_dialog, &PreferencesDialog::slotIconTypeChange );
    connect( m_preferences, &Preferences::signalIconDataChange, m_pref_dialog, &PreferencesDialog::slotIconDataChange );
    connect( m_preferences, &Preferences::signalShowNumberChange, m_pref_dialog, &PreferencesDialog::slotShowNumberChange );
    connect( m_preferences, &Preferences::signalNumberColorChange, m_pref_dialog, &PreferencesDialog::slotNumberColorChange );
    connect( m_preferences, &Preferences::signalNumberSizeChange, m_pref_dialog, &PreferencesDialog::slotNumberSizeChange );
    connect( m_preferences, &Preferences::signalNumberAlignmentChange, m_pref_dialog, &PreferencesDialog::slotNumberAlignmentChange );
    connect( m_preferences, &Preferences::signalNumberMarginsChange, m_pref_dialog, &PreferencesDialog::slotNumberMarginsChange );
    connect( m_preferences, &Preferences::signalCountTypeChange, m_pref_dialog, &PreferencesDialog::slotCountTypeChange );
    connect( m_preferences, &Preferences::signalMinimizeTypeChange, m_pref_dialog, &PreferencesDialog::slotMinimizeTypeChange );
    connect( m_preferences, &Preferences::signalStartMinimizedChange, m_pref_dialog, &PreferencesDialog::slotStartMinimizedChange );
    connect( m_preferences, &Preferences::signalRestoreWindowPositionsChange, m_pref_dialog, &PreferencesDialog::slotRestoreWindowPositionsChange );
    connect( m_preferences, &Preferences::signalCloseTypeChange, m_pref_dialog, &PreferencesDialog::slotCloseTypeChange );
    connect( m_preferences, &Preferences::signalThemeChange, m_pref_dialog, &PreferencesDialog::slotThemeChange );
    connect( m_preferences, &Preferences::signalDebugChange, m_pref_dialog, &PreferencesDialog::slotDebugChange );

    connect( m_preferences, &Preferences::signalDefaultIconTypeChange, m_link, &SysTrayXLink::slotDefaultIconTypeChange );
    connect( m_preferences, &Preferences::signalDefaultIconDataChange, m_link, &SysTrayXLink::slotDefaultIconDataChange );
    connect( m_preferences, &Preferences::signalHideDefaultIconChange, m_link, &SysTrayXLink::slotHideDefaultIconChange );
    connect( m_preferences, &Preferences::signalIconTypeChange, m_link, &SysTrayXLink::slotIconTypeChange );
    connect( m_preferences, &Preferences::signalIconDataChange, m_link, &SysTrayXLink::slotIconDataChange );
    connect( m_preferences, &Preferences::signalShowNumberChange, m_link, &SysTrayXLink::slotShowNumberChange );
    connect( m_preferences, &Preferences::signalNumberColorChange, m_link, &SysTrayXLink::slotNumberColorChange );
    connect( m_preferences, &Preferences::signalNumberSizeChange, m_link, &SysTrayXLink::slotNumberSizeChange );
    connect( m_preferences, &Preferences::signalNumberAlignmentChange, m_link, &SysTrayXLink::slotNumberAlignmentChange );
    connect( m_preferences, &Preferences::signalNumberMarginsChange, m_link, &SysTrayXLink::slotNumberMarginsChange );
    connect( m_preferences, &Preferences::signalCountTypeChange, m_link, &SysTrayXLink::slotCountTypeChange );
    connect( m_preferences, &Preferences::signalMinimizeTypeChange, m_link, &SysTrayXLink::slotMinimizeTypeChange );
    connect( m_preferences, &Preferences::signalStartMinimizedChange, m_link, &SysTrayXLink::slotStartMinimizedChange );
    connect( m_preferences, &Preferences::signalRestoreWindowPositionsChange, m_link, &SysTrayXLink::slotRestoreWindowPositionsChange );
    connect( m_preferences, &Preferences::signalCloseTypeChange, m_link, &SysTrayXLink::slotCloseTypeChange );
    connect( m_preferences, &Preferences::signalThemeChange, m_link, &SysTrayXLink::slotThemeChange );
    connect( m_preferences, &Preferences::signalDebugChange, m_link, &SysTrayXLink::slotDebugChange );
    connect( m_preferences, &Preferences::signalHideDefaultIconChange, this,  &SysTrayX::slotSelectIconObjectPref );

    connect( m_preferences, &Preferences::signalDebugChange, m_debug, &DebugWidget::slotDebugChange );

    /*
     *  Connect link signals
     */
    connect( m_link, &SysTrayXLink::signalAddOnShutdown, this, &SysTrayX::slotAddOnShutdown );
    connect( m_link, &SysTrayXLink::signalErrorAddOnShutdown, this, &SysTrayX::slotErrorAddOnShutdown );
    connect( m_link, &SysTrayXLink::signalWindowState, m_win_ctrl, &WindowCtrl::slotWindowState );
    connect( m_link, &SysTrayXLink::signalUnreadMail, this, &SysTrayX::slotSetUnreadMail );
    connect( m_link, &SysTrayXLink::signalVersion, this, &SysTrayX::slotVersion );
    connect( m_link, &SysTrayXLink::signalKdeIntegration, this, &SysTrayX::slotSelectIconObject );
    connect( m_link, &SysTrayXLink::signalLocale, this, &SysTrayX::slotLoadLanguage );

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

///*
    slotLoadLanguage( "en-US" );
    //slotLoadLanguage( "it" );
    //slotLoadLanguage( "nl" );
    //slotLoadLanguage( "pt-BR" );
    //slotLoadLanguage( "ru" );
    slotSelectIconObject( false );

    slotSetUnreadMail( 10 );
//*/
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
    connect( m_pref_action, &QAction::triggered, m_pref_dialog, &PreferencesDialog::showNormal );

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
        connect( m_preferences, &Preferences::signalShowNumberChange, m_tray_icon, &SysTrayXIcon::slotShowNumberChange );
        connect( m_preferences, &Preferences::signalNumberColorChange, m_tray_icon, &SysTrayXIcon::slotNumberColorChange );
        connect( m_preferences, &Preferences::signalNumberSizeChange, m_tray_icon, &SysTrayXIcon::slotNumberSizeChange );
        connect( m_preferences, &Preferences::signalNumberAlignmentChange, m_tray_icon, &SysTrayXIcon::slotNumberAlignmentChange );
        connect( m_preferences, &Preferences::signalNumberMarginsChange, m_tray_icon, &SysTrayXIcon::slotNumberMarginsChange );
        connect( m_preferences, &Preferences::signalThemeChange, m_tray_icon, &SysTrayXIcon::slotThemeChange );

        connect( m_link, &SysTrayXLink::signalUnreadMail, m_tray_icon, &SysTrayXIcon::slotSetUnreadMail );

        connect( this, &SysTrayX::signalUnreadMail, m_tray_icon, &SysTrayXIcon::slotSetUnreadMail );

        /*
         *  Show it
         */
        m_tray_icon->show();

        QTimer::singleShot(500, this, &SysTrayX::resendUnreadMail);
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
        disconnect( m_preferences, &Preferences::signalShowNumberChange, m_tray_icon, &SysTrayXIcon::slotShowNumberChange );
        disconnect( m_preferences, &Preferences::signalNumberColorChange, m_tray_icon, &SysTrayXIcon::slotNumberColorChange );
        disconnect( m_preferences, &Preferences::signalNumberSizeChange, m_tray_icon, &SysTrayXIcon::slotNumberSizeChange );
        disconnect( m_preferences, &Preferences::signalNumberAlignmentChange, m_tray_icon, &SysTrayXIcon::slotNumberAlignmentChange );
        disconnect( m_preferences, &Preferences::signalNumberMarginsChange, m_tray_icon, &SysTrayXIcon::slotNumberMarginsChange );
        disconnect( m_preferences, &Preferences::signalThemeChange, m_tray_icon, &SysTrayXIcon::slotThemeChange );

        disconnect( m_link, &SysTrayXLink::signalUnreadMail, m_tray_icon, &SysTrayXIcon::slotSetUnreadMail );

        disconnect( this, &SysTrayX::signalUnreadMail, m_tray_icon, &SysTrayXIcon::slotSetUnreadMail );

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
        connect( m_preferences, &Preferences::signalShowNumberChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotShowNumberChange );
        connect( m_preferences, &Preferences::signalNumberColorChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNumberColorChange );
        connect( m_preferences, &Preferences::signalNumberSizeChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNumberSizeChange );
        connect( m_preferences, &Preferences::signalNumberAlignmentChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNumberAlignmentChange );
        connect( m_preferences, &Preferences::signalNumberMarginsChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNumberMarginsChange );
        connect( m_preferences, &Preferences::signalThemeChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotThemeChange );

        connect( m_link, &SysTrayXLink::signalUnreadMail, m_kde_tray_icon, &SysTrayXStatusNotifier::slotSetUnreadMail );

        connect( this, &SysTrayX::signalUnreadMail, m_kde_tray_icon, &SysTrayXStatusNotifier::slotSetUnreadMail );

        /*
         *  Show
         */
        QTimer::singleShot(500, this, &SysTrayX::resendUnreadMail);
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
        disconnect( m_preferences, &Preferences::signalShowNumberChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotShowNumberChange );
        disconnect( m_preferences, &Preferences::signalNumberColorChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNumberColorChange );
        disconnect( m_preferences, &Preferences::signalNumberSizeChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNumberSizeChange );
        disconnect( m_preferences, &Preferences::signalNumberAlignmentChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNumberAlignmentChange );
        disconnect( m_preferences, &Preferences::signalNumberMarginsChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotNumberMarginsChange );
        disconnect( m_preferences, &Preferences::signalThemeChange, m_kde_tray_icon, &SysTrayXStatusNotifier::slotThemeChange );

        disconnect( m_link, &SysTrayXLink::signalUnreadMail, m_kde_tray_icon, &SysTrayXStatusNotifier::slotSetUnreadMail );

        disconnect( this, &SysTrayX::signalUnreadMail, m_kde_tray_icon, &SysTrayXStatusNotifier::slotSetUnreadMail );

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
 *  Resend unread mail
 */
void    SysTrayX::resendUnreadMail()
{
    emit signalUnreadMail( m_unread_mail );
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
 *  Handle unread mail signal
 */
void    SysTrayX::slotSetUnreadMail( int unread )
{
    m_unread_mail = unread;
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
        m_translator.load( locale_path, ":/languages/" );
//        bool status = m_translator.load( locale_path, ":/languages/" );
//        emit signalConsole( QString( "Language loaded %1").arg(status));
        qApp->installTranslator( &m_translator );
    }
}
