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

/**
 * @brief Preferences.  Constructor.
 */
Preferences::Preferences( QObject *parent ) : QObject( parent )
{
    /*
     *  Initialize
     */
    m_app_pref_changed = false;

    m_icon_type = PREF_BLANK_ICON;
    m_icon_mime = "image/png";
    m_icon_data = QByteArray();

    m_show_number = true;
    m_number_color = "#000000";

    m_hide_minimize = true;
    m_start_minimized = false;

    m_poll_startup_delay = 30;
    m_poll_interval = 30;

    m_debug = false;

    m_version_major = QLatin1String( APP_VERSION_MAJOR );
    m_version_minor = QLatin1String( APP_VERSION_MINOR );
    m_version_patch = QLatin1String( APP_VERSION_PATCH );

    m_version_build = QLatin1String( APP_BUILD );
    m_version_hash = QLatin1String( APP_GITHASH );
    m_version_branch = QLatin1String( APP_GITBRANCH );
}


/*
 *  Get the icon type.
 */
Preferences::IconType Preferences::getIconType() const
{
    return m_icon_type;
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
 *  Get the hide on minimize pref.
 */
bool    Preferences::getHideOnMinimize() const
{
    return m_hide_minimize;
}


/*
 *  Set the hide on minimize pref.
 */
void    Preferences::setHideOnMinimize( bool state )
{
    if( m_hide_minimize != state )
    {
        m_hide_minimize = state;

        /*
         *  Tell the world the new preference
         */
        emit signalHideOnMinimizeChange();
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
 *  Get the poll startup delay.
 */
int    Preferences::getPollStartupDelay() const
{
    return m_poll_startup_delay;
}


/*
 *  Set the startup poll delay.
 */
void    Preferences::setPollStartupDelay( int val )
{
    if( m_poll_startup_delay != val )
    {
        m_poll_startup_delay = val;

        /*
         *  Tell the world the new preference
         */
        emit signalPollStartupDelayChange();
    }
}


/*
 *  Get the poll interval.
 */
int    Preferences::getPollInterval() const
{
    return m_poll_interval;
}


/*
 *  Set the startup poll delay.
 */
void    Preferences::setPollInterval( int val )
{
    if( m_poll_interval != val )
    {
        m_poll_interval = val;

        /*
         *  Tell the world the new preference
         */
        emit signalPollIntervalChange();
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
