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


/*
 *  Initialize statics
 */
const QString   Preferences::STATE_NORMAL_STR = "normal";
const QString   Preferences::STATE_MINIMIZED_STR = "minimized";
const QString   Preferences::STATE_MAXIMIZED_STR = "maximized";
const QString   Preferences::STATE_FULLSCREEN_STR = "fullscreen";
const QString   Preferences::STATE_DOCKED_STR = "docked";
const QString   Preferences::STATE_MINIMIZED_ALL_STR = "minimized_all";
const QString   Preferences::STATE_MINIMIZED_ALL_STARTUP_STR = "minimized_all_startup";

const QStringList   Preferences::WindowStateString = {
           "Unknown",
           STATE_NORMAL_STR,
           STATE_MINIMIZED_STR,
           STATE_MAXIMIZED_STR,
           STATE_FULLSCREEN_STR,
           STATE_DOCKED_STR,
           STATE_MINIMIZED_ALL_STR,
           STATE_MINIMIZED_ALL_STARTUP_STR
       };

/**
 * @brief Preferences.  Constructor.
 */
Preferences::Preferences( QObject *parent ) : QObject( parent )
{
    /*
     *  Initialize
     */
    m_app_pref_changed = false;

    m_default_icon_type = PREF_DEFAULT_ICON_DEFAULT;
    m_default_icon_mime = "image/png";
    m_default_icon_data = QByteArray();
    m_hide_default_icon = false;

    m_icon_type = PREF_BLANK_ICON;
    m_icon_mime = "image/png";
    m_icon_data = QByteArray();

    m_show_number = true;
    m_number_color = "#000000";
    m_number_size = 10;
    m_count_type = PREF_COUNT_UNREAD;
    m_startup_delay = 5;
    m_number_alignment = 4;
    m_number_margins = QMargins();

    m_minimize_type = PREF_MINIMIZE_METHOD_1;
    m_start_minimized = false;
    m_close_type = PREF_MINIMIZE_MAIN_CLOSE_CHILDREN_WINDOWS;

    m_restore_window_positions = false;

    m_debug = false;

    m_version_major = QLatin1String( APP_VERSION_MAJOR );
    m_version_minor = QLatin1String( APP_VERSION_MINOR );
    m_version_patch = QLatin1String( APP_VERSION_PATCH );

    m_version_build = QLatin1String( APP_BUILD );
    m_version_hash = QLatin1String( APP_GITHASH );
    m_version_branch = QLatin1String( APP_GITBRANCH );

    m_theme = PREF_THEME_LIGHT;
}


/*
 *  Get the icon mime.
 */
bool    Preferences::getAppPrefChanged() const
{
    return m_app_pref_changed;
}


/*
 *  Control the sending of preferences changes to the add-on
 */
void    Preferences::setAppPrefChanged( bool state )
{
    if( m_app_pref_changed != state )
    {
        m_app_pref_changed = state;
    }
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

    /*
     *  Tell the world the new preference
     */
    emit signalBrowserVersion();
}


/*
 *  Set the browser
 */
void    Preferences::setBrowserBuildID( const QString buildID )
{
    m_browser_buildID = buildID;
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
 *  Get the number color.
 */
QString    Preferences::getNumberColor() const
{
    return m_number_color;
}


/*
 *  Set the enable number state.
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
 *  Set the enable number state.
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
bool    Preferences::getStartMinimized() const
{
    return m_start_minimized;
}


/*
 *  Set the start minimized pref.
 */
void    Preferences::setStartMinimized( bool state )
{
    if( m_start_minimized != state )
    {
        m_start_minimized = state;

        /*
         *  Tell the world the new preference
         */
        emit signalStartMinimizedChange();
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
 *  Get the theme pref.
 */
Preferences::Theme    Preferences::getTheme() const
{
    return m_theme;
}


/*
 *  Set the theme pref.
 */
void    Preferences::setTheme( Theme theme )
{
    if( m_theme != theme )
    {
        m_theme = theme;

        /*
         *  Tell the world the new preference
         */
        emit signalThemeChange();
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
