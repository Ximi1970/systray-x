#include "preferences.h"

/*
 *	Local includes
 */


/*
 *  System includes
 */

/*
 *	Qt includes
 */
#include <QGuiApplication>


/*
 *  Initialize statics
 */
const QString   Preferences::STATE_NORMAL_STR = "normal";
const QString   Preferences::STATE_MINIMIZED_STR = "minimized";
const QString   Preferences::STATE_MAXIMIZED_STR = "maximized";
const QString   Preferences::STATE_FULLSCREEN_STR = "fullscreen";
const QString   Preferences::STATE_DOCKED_STR = "docked";
const QString   Preferences::STATE_MINIMIZED_STARTUP_STR = "minimized_startup";
const QString   Preferences::STATE_DOCKED_STARTUP_STR = "docked_startup";

const QStringList   Preferences::WindowStateString = {
           "Unknown",
           STATE_NORMAL_STR,
           STATE_MINIMIZED_STR,
           STATE_MAXIMIZED_STR,
           STATE_FULLSCREEN_STR,
           STATE_DOCKED_STR,
           STATE_MINIMIZED_STARTUP_STR,
           STATE_DOCKED_STARTUP_STR
       };

/**
 * @brief Preferences.  Constructor.
 */
Preferences::Preferences( QObject *parent ) : QObject( parent )
{
    /*
     *  Initialize
     */
    m_minimize_type = PREF_MINIMIZE_METHOD_1;
    m_close_type = PREF_MINIMIZE_MAIN_TRAY_CLOSE_CHILDREN_WINDOWS;
    m_minimize_icon_type = PREF_MINIMIZE_TRAY_ICON;

    m_startup_type = PREF_START_DEFAULT;
    m_restore_window_positions = false;

    m_default_icon_type = PREF_DEFAULT_ICON_DEFAULT;
    m_default_icon_mime = "image/png";
    m_default_icon_data = QByteArray();
    m_hide_default_icon = false;

    m_icon_type = PREF_BLANK_ICON;
    m_icon_mime = "image/png";
    m_icon_data = QByteArray();

    m_invert_icon = false;

    m_show_number = true;
    m_show_new_indicator = false;
    m_number_color = "#000000";
    m_number_size = 10;
    m_count_type = PREF_COUNT_UNREAD;
    m_startup_delay = 5;
    m_api_count_method = false;
    m_number_alignment = 4;
    m_number_margins = QMargins();
    m_new_indicator_type = PREF_NEW_INDICATOR_SHADE;
    m_new_shade_color = "#80ff8000";

    m_debug = false;

    m_version_major = QLatin1String( APP_VERSION_MAJOR );
    m_version_minor = QLatin1String( APP_VERSION_MINOR );
    m_version_patch = QLatin1String( APP_VERSION_PATCH );

    m_version_build = QLatin1String( APP_BUILD );
    m_version_hash = QLatin1String( APP_GITHASH );
    m_version_branch = QLatin1String( APP_GITBRANCH );

    m_browser_version = "0.0.0";

    m_start_app = "";
    m_start_app_args = "";
    m_close_app = "";
    m_close_app_args = "";

    m_show_hide_shortcut = QKeySequence();

    /*
     *  Initialize the system parameters
     */
    m_platform = QGuiApplication::platformName();
    m_xdg_session_desktop = getenv( "XDG_SESSION_DESKTOP" );
    m_xdg_current_desktop = getenv( "XDG_CURRENT_DESKTOP" );
    m_xdg_session_type = getenv( "XDG_SESSION_TYPE" );

    /*
     *  Set the options
     */
#if ( defined( Q_OS_UNIX ) && defined( NO_KDE_INTEGRATION ) ) || defined( Q_OS_WIN )
    m_kde_integration_option = false;
#else
    m_kde_integration_option = true;
#endif

#if ( defined( Q_OS_UNIX ) && defined( NO_SHORTCUTS ) )
    m_shortcuts_option = false;
#else
    m_shortcuts_option = true;
#endif
/*
    // XDG_SESSION_DESKTOP
    // XDG_CURRENT_DESKTOP
    // XDG_SESSION_TYPE

    // Leap 15.4 "KDE", "default", "gnome-classic", "gnome-xorg", "icewm", "xfce"
    // Leap 15.4 "KDE", "GNOME", "GNOME-Classic:GNOME", "GNOME", "ICEWM", "XFCE"
    // Leap 15.4 "x11", "wayland", "x11", "x11", "x11", x11

    // Leap 15.5 "KDE", "default", "gnome-classic", "gnome-xorg", "icewm", "xfce"
    // Leap 15.5 "KDE", "GNOME", "GNOME-Classic:GNOME", "GNOME", "ICEWM", "XFCE"
    // Leap 15.5 "x11", "wayland", "x11", "x11", "x11", x11

    // Ubuntu 18.03:    "ubuntu", "ubuntu-wayland"
    // Ubuntu 18.03:    "ubuntu:GNOME", "ubuntu:GNOME"
    // Ubuntu 18.03:    "x11", "wayland"

    // Xubuntu 18.03:    "xubuntu",
    // Xubuntu 18.03:    "XFCE",
    // Xubuntu 18.03:    "x11",

    // Kubuntu 18.03:    "KDE",
    // Kubuntu 18.03:    "KDE",
    // Kubuntu 18.03:    "x11",

    // Lubuntu 18.03:    "Lubuntu",
    // Lubuntu 18.03:    "LXDE",
    // Lubuntu 18.03:    "x11",

    // Ubuntu 23.10:    "ubuntu", "ubuntu-xorg"
    // Ubuntu 23.10:    "ubuntu:GNOME", "ubuntu:GNOME"
    // Ubuntu 23.10:    "wayland", "x11"

    // Xubuntu 23.10:    "xubuntu",
    // Xubuntu 23.10:    "XFCE",
    // Xubuntu 23.10:    "x11",

    // Kubuntu 23.10:    "KDE",
    // Kubuntu 23.10:    "KDE",
    // Kubuntu 23.10:    "x11",

    // Lubuntu 23.10:    "LXQt",
    // Lubuntu 23.10:    "LXQt",
    // Lubuntu 23.10:    "x11",

    // Debian 10:   "gnome", "KDE"
    // Debian 10:   "GNOME", "KDE"
    // Debian 10:   "wayland", "x11"

    // Debian 12:   "gnome", "KDE"
    // Debian 12:   "GNOME", "KDE"
    // Debian 12:   "wayland", "x11"


    // Fedora 36:   "gnome"
    // Fedora 36:   "GNOME"
    // Fedora 36:   "wayland"

    // Fedora 38:   "gnome", "gnome-xorg", "mate"
    // Fedora 38:   "GNOME", "GNOME", "MATE"
    // Fedora 38:   "wayland", "x11", "x11"

    // Manjaro:     "gnome", -
    // Manjaro:     "GNOME", "KDE"
    // Manjaro:     "wayland", -

    // Arch:        "gnome", "gnome-xorg", "xfce"
    // Arch:        "GNOME", "GNOME", "XFCE"
    // Arch:        "wayland", "x11", "x11"

    // Mint:        "xfce"
    // Mint:        "XFCE"
    // Mint:        "x11"

    //  Qt platform:        "xcb", "wayland", "windows", ...
    //  Session desktop:    "KDE", "gnome", ...
    //  Session type:       "x11", "wayland", ...

    if( m_session_type == "x11" && m_session_desktop == "KDE" )
    {
        m_kde_integration_option = true;
    }
    else
    {
        m_kde_integration_option = false;
    }

    if( m_platform == "wayland" || m_session_type == "wayland" || m_session_desktop == "GNOME" )
    {
        m_shortcuts_option = false;
    }
    else
    {
        m_shortcuts_option = true;
    }
*/
}


/*
 *  Display some debug info
 */
void    Preferences::displayDebug()
{
    emit signalConsole( QString( "Platform: %1" ).arg( m_platform ) );
    emit signalConsole( QString( "Session current desktop: %1" ).arg( m_xdg_current_desktop ) );
    emit signalConsole( QString( "Session desktop: %1" ).arg( m_xdg_session_desktop ) );
    emit signalConsole( QString( "Session type: %1" ).arg( m_xdg_session_type ) );
}


/*
 *  Set the platform os
 */
void    Preferences::setPlatformOs( const QString os )
{
   m_platform_os = os;
}


/*
 *  Set the platform arch
 */
void    Preferences::setPlatformArch( const QString arch )
{
    m_platform_arch = arch;
}


/*
 *  Set the platform native arch
 */
void    Preferences::setPlatformNaclArch( const QString nacl_arch )
{
    m_platform_nacl_arch = nacl_arch;
}


/*
 *  Set the browser name
 */
void    Preferences::setBrowserName( const QString name )
{
    m_browser_name = name;
}


/*
 *  Set the browser
 */
void    Preferences::setBrowserVendor( const QString vendor )
{
    m_browser_vendor = vendor;
}


/*
 *  Get the browser
 */
const QString&    Preferences::getBrowserVersion() const
{
    return m_browser_version;
}


/*
 *  Set the browser
 */
void    Preferences::setBrowserVersion( const QString version )
{
    m_browser_version = version;
}


/*
 *  Set the browser
 */
void    Preferences::setBrowserBuildID( const QString buildID )
{
    m_browser_buildID = buildID;
}


/*
 *  Get the software version.
 */
QString    Preferences::getVersion() const
{
    return m_version_major + "." + m_version_minor + "." + m_version_patch;
}


/*
 *  Get the number of commits.
 */
QString    Preferences::getBuild() const
{
    return m_version_build;
}


/*
 *  Get the git hash.
 */
QString    Preferences::getHash() const
{
    return m_version_hash;
}


/*
 *  Get the software version.
 */
QString    Preferences::getBranch() const
{
    return m_version_branch;
}


/*
 *  Get the minimize type
 */
Preferences::MinimizeType Preferences::getMinimizeType() const
{
    return m_minimize_type;
}


/*
 *  Set the minimize type.
 */
void    Preferences::setMinimizeType( MinimizeType minimize_type )
{
    if( m_minimize_type != minimize_type)
    {
        m_minimize_type = minimize_type;

        /*
         *  Tell the world the new preference
         */
        emit signalMinimizeTypeChange();
    }
}


/*
 *  Get the close type pref.
 */
Preferences::CloseType    Preferences::getCloseType() const
{
    return m_close_type;
}


/*
 *  Set the close type pref.
 */
void    Preferences::setCloseType( CloseType close_type )
{
    if( m_close_type != close_type )
    {
        m_close_type = close_type;

        /*
         *  Tell the world the new preference
         */
        emit signalCloseTypeChange();
    }
}


/*
 *  Get the minimize type
 */
Preferences::MinimizeIconType Preferences::getMinimizeIconType() const
{
    return m_minimize_icon_type;
}


/*
 *  Set the minimize type.
 */
void    Preferences::setMinimizeIconType( MinimizeIconType minimize_icon_type )
{
    if( m_minimize_icon_type != minimize_icon_type)
    {
        m_minimize_icon_type = minimize_icon_type;

        /*
         *  Tell the world the new preference
         */
        emit signalMinimizeIconTypeChange();
    }
}


/*
 *  Get the start minmized pref.
 */
Preferences::StartupType    Preferences::getStartupType() const
{
    return m_startup_type;
}


/*
 *  Set the startup type pref.
 */
void    Preferences::setStartupType( Preferences::StartupType startup_type )
{
    if( m_startup_type != startup_type )
    {
        m_startup_type = startup_type;

        /*
         *  Tell the world the new preference
         */
        emit signalStartupTypeChange();
    }
}


/*
 *  Get the start minmized pref.
 */
bool    Preferences::getRestoreWindowPositions() const
{
    return m_restore_window_positions;
}


/*
 *  Set the start minimized pref.
 */
void    Preferences::setRestoreWindowPositions( bool state )
{
    if( m_restore_window_positions != state )
    {
        m_restore_window_positions = state;

        /*
         *  Tell the world the new preference
         */
        emit signalRestoreWindowPositionsChange();
    }
}


/*
 *  Get the default icon type.
 */
Preferences::DefaultIconType Preferences::getDefaultIconType() const
{
    return m_default_icon_type;
}


/*
 *  Set the default icon type.
 */
void    Preferences::setDefaultIconType( DefaultIconType icon_type )
{
    if( m_default_icon_type != icon_type)
    {
        m_default_icon_type = icon_type;

        /*
         *  Tell the world the new preference
         */
        emit signalDefaultIconTypeChange();
    }
}


/*
 *  Get the default icon mime.
 */
const QString&  Preferences::getDefaultIconMime() const
{
    return m_icon_mime;
}


/*
 *  Set the default icon mime.
 */
void    Preferences::setDefaultIconMime( const QString& icon_mime )
{
    m_default_icon_mime = icon_mime;
}


/*
 *  Get the default icon data.
 */
const QByteArray&   Preferences::getDefaultIconData() const
{
    return m_default_icon_data;
}


/*
 *  Set the icon data.
 */
void    Preferences::setDefaultIconData( const QByteArray& icon_data )
{

    if( m_default_icon_data != icon_data )
    {
        m_default_icon_data = icon_data;

        /*
         *  Tell the world the new preference
         */
        emit signalDefaultIconDataChange();
    }
}


/*
 *  Get the hide default icon state.
 */
bool    Preferences::getHideDefaultIcon() const
{
    return m_hide_default_icon;
}


/*
 *  Set the hide default icon state.
 */
void    Preferences::setHideDefaultIcon( bool hide )
{
    if( m_hide_default_icon != hide)
    {
        m_hide_default_icon = hide;

        /*
         *  Tell the world the new preference
         */
        emit signalHideDefaultIconChange();
    }
}


/*
 *  Get the icon type.
 */
Preferences::IconType Preferences::getIconType() const
{
    return m_icon_type;
}


/*
 *  Set the icon type.
 */
void    Preferences::setIconType( IconType icon_type )
{
    if( m_icon_type != icon_type)
    {
        m_icon_type = icon_type;

        /*
         *  Tell the world the new preference
         */
        emit signalIconTypeChange();
    }
}


/*
 *  Get the icon mime.
 */
const QString&  Preferences::getIconMime() const
{
    return m_icon_mime;
}


/*
 *  Set the icon mime.
 */
void    Preferences::setIconMime( const QString& icon_mime )
{
    m_icon_mime = icon_mime;
}


/*
 *  Get the icon data.
 */
const QByteArray&   Preferences::getIconData() const
{
    return m_icon_data;
}


/*
 *  Set the icon data.
 */
void    Preferences::setIconData( const QByteArray& icon_data )
{

    if( m_icon_data != icon_data )
    {
        m_icon_data = icon_data;

        /*
         *  Tell the world the new preference
         */
        emit signalIconDataChange();
    }
}


/*
 *  Get the invert icon pref.
 */
bool    Preferences::getInvertIcon() const
{
    return m_invert_icon;
}


/*
 *  Set the invert icon pref.
 */
void    Preferences::setInvertIcon( bool state )
{
    if( m_invert_icon != state )
    {
        m_invert_icon = state;

        /*
         *  Tell the world the new preference
         */
        emit signalInvertIconChange();
    }
}


/*
 *  Get the enable number state.
 */
bool    Preferences::getShowNumber() const
{
    return m_show_number;
}


/*
 *  Set the enable number state.
 */
void    Preferences::setShowNumber( bool state )
{
    if( m_show_number != state )
    {
        m_show_number = state;

        /*
         *  Tell the world the new preference
         */
        emit signalShowNumberChange();
    }
}


/*
 *  Get the show new indicator state.
 */
bool    Preferences::getShowNewIndicator() const
{
    return m_show_new_indicator;
}


/*
 *  Set the show new indicator state.
 */
void    Preferences::setShowNewIndicator( bool state )
{
    if( m_show_new_indicator != state )
    {
        m_show_new_indicator = state;

        /*
         *  Tell the world the new preference
         */
        emit signalShowNewIndicatorChange();
    }
}


/*
 *  Get the count type.
 */
Preferences::CountType Preferences::getCountType() const
{
    return m_count_type;
}


/*
 *  Set the count type.
 */
void    Preferences::setCountType( CountType count_type )
{
    if( m_count_type != count_type)
    {
        m_count_type = count_type;

        /*
         *  Tell the world the new preference
         */
        emit signalCountTypeChange();
    }
}


/*
 *  Get the startup delay.
 */
int Preferences::getStartupDelay() const
{
    return m_startup_delay;
}


/*
 *  Set the startup delay.
 */
void    Preferences::setStartupDelay( int delay )
{
    if( m_startup_delay != delay )
    {
        m_startup_delay = delay;

        /*
         *  Tell the world the new preference
         */
        emit signalStartupDelayChange();
    }
}


/*
 *  Get the number color.
 */
QString    Preferences::getNumberColor() const
{
    return m_number_color;
}


/*
 *  Set the number color.
 */
void    Preferences::setNumberColor( QString color )
{
    if( m_number_color != color )
    {
        m_number_color = color;

        /*
         *  Tell the world the new preference
         */
        emit signalNumberColorChange();
    }
}


/*
 *  Get the number size.
 */
int Preferences::getNumberSize() const
{
    return m_number_size;
}


/*
 *  Set the number size.
 */
void    Preferences::setNumberSize( int size )
{
    if( m_number_size != size )
    {
        m_number_size = size;

        /*
         *  Tell the world the new preference
         */
        emit signalNumberSizeChange();
    }
}


/*
 *  Get the number alignment.
 */
int Preferences::getNumberAlignment() const
{
    return m_number_alignment;
}


/*
 *  Set the number alignment.
 */
void    Preferences::setNumberAlignment( int alignment )
{
    if( m_number_alignment != alignment)
    {
        m_number_alignment = alignment;

        /*
         *  Tell the world the new preference
         */
        emit signalNumberAlignmentChange();
    }
}


/*
 *  Get the number margins.
 */
QMargins Preferences::getNumberMargins() const
{
    return m_number_margins;
}


/*
 *  Set the number margins.
 */
void    Preferences::setNumberMargins( QMargins margins )
{
    if( m_number_margins != margins)
    {
        m_number_margins = margins;

        /*
         *  Tell the world the new preference
         */
        emit signalNumberMarginsChange();
    }
}


/*
 *  Get the new indicator type.
 */
Preferences::NewIndicatorType Preferences::getNewIndicatorType() const
{
    return m_new_indicator_type;
}


/*
 *  Set the new indicator type.
 */
void    Preferences::setNewIndicatorType( NewIndicatorType new_indicator_type )
{
    if( m_new_indicator_type != new_indicator_type)
    {
        m_new_indicator_type = new_indicator_type;

        /*
         *  Tell the world the new preference
         */
        emit signalNewIndicatorTypeChange();
    }
}


/*
 *  Get the new shade color.
 */
QString    Preferences::getNewShadeColor() const
{
    return m_new_shade_color;
}


/*
 *  Set the new shade color.
 */
void    Preferences::setNewShadeColor( QString color )
{
    if( m_new_shade_color != color )
    {
        m_new_shade_color = color;

        /*
         *  Tell the world the new preference
         */
        emit signalNewShadeColorChange();
    }
}


/*
 *  Get the start application.
 */
QString    Preferences::getStartApp() const
{

    return m_start_app;
}


/*
 *  Set the start application.
 */
void    Preferences::setStartApp( QString app )
{
    if( m_start_app != app )
    {
        m_start_app = app;

        /*
         *  Tell the world the new preference
         */
        emit signalStartAppChange();
    }
}


/*
 *  Get the start application arguments.
 */
QString    Preferences::getStartAppArgs() const
{

    return m_start_app_args;
}


/*
 *  Set the start application arguments.
 */
void    Preferences::setStartAppArgs( QString args )
{
    if( m_start_app_args != args )
    {
        m_start_app_args = args;

        /*
         *  Tell the world the new preference
         */
        emit signalStartAppArgsChange();
    }
}


/*
 *  Get the close application.
 */
QString    Preferences::getCloseApp() const
{

    return m_close_app;
}


/*
 *  Set the close application.
 */
void    Preferences::setCloseApp( QString app )
{
    if( m_close_app != app )
    {
        m_close_app = app;

        /*
         *  Tell the world the new preference
         */
        emit signalCloseAppChange();
    }
}


/*
 *  Get the close application arguments.
 */
QString    Preferences::getCloseAppArgs() const
{

    return m_close_app_args;
}


/*
 *  Set the close application arguments.
 */
void    Preferences::setCloseAppArgs( QString args )
{
    if( m_close_app_args != args )
    {
        m_close_app_args = args;

        /*
         *  Tell the world the new preference
         */
        emit signalCloseAppArgsChange();
    }
}


/*
 *  Get the API count method state.
 */
bool    Preferences::getApiCountMethod() const
{
    return m_api_count_method;
}


/*
 *  Set the API count method state.
 */
void    Preferences::setApiCountMethod( bool state )
{
    if( m_api_count_method != state )
    {
        m_api_count_method = state;

        /*
         *  Tell the world the new preference
         */
        emit signalApiCountMethodChange();
    }
}


/*
 *  Get the show / hide shortcut key sequence.
 */
QKeySequence    Preferences::getShowHideShortcut() const
{
    return m_show_hide_shortcut;
}


/*
 *  Set the show / hide shortcut key sequence.
 */
void    Preferences::setShowHideShortcut( QKeySequence key_seq )
{
    if( m_show_hide_shortcut != key_seq )
    {
        m_show_hide_shortcut = key_seq;

        /*
         *  Tell the world the new preference
         */
        emit signalShowHideShortcutChange();
    }
}


/*
 *  Get the KDE integration option.
 */
bool    Preferences::getKdeIntegrationOption() const
{
    return m_kde_integration_option;
}


/*
 *  Get the KDE integration option.
 */
void    Preferences::setKdeIntegrationOption( bool state )
{
    if( m_kde_integration_option != state )
    {
        m_kde_integration_option = state;

        /*
         *  Tell the world the new option
         */
        //emit signalOptionChange();
    }
}


/*
 *  Get the shortcuts option.
 */
bool    Preferences::getShortcutsOption() const
{
    return m_shortcuts_option;
}


/*
 *  Get the shortcuts option.
 */
void    Preferences::setShortcutsOption( bool state )
{
    if( m_shortcuts_option != state )
    {
        m_shortcuts_option = state;

        /*
         *  Tell the world the new option
         */
        //emit signalOptionChange();
    }
}


/*
 *  Get the debug state.
 */
bool    Preferences::getDebug() const
{
    return m_debug;
}


/*
 *  Set the debug state.
 */
void    Preferences::setDebug( bool state )
{
    if( m_debug != state )
    {
        m_debug = state;

        /*
         *  Tell the world the new preference
         */
        emit signalDebugChange();
    }
}
