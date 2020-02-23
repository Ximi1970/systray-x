#include "preferences.h"

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

    m_hide_minimize = true;
    m_minimize_close = true;

    m_debug = false;
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
 *  Get the hide on minimize state.
 */
bool    Preferences::getHideOnMinimize() const
{
    return m_hide_minimize;
}


/*
 *  Set the minimize on close state.
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
 *  Get the minimuze on close state.
 */
bool    Preferences::getMinimizeOnClose() const
{
    return m_minimize_close;
}


/*
 *  Set the minimize on close data.
 */
void    Preferences::setMinimizeOnClose( bool state )
{
    if( m_minimize_close != state )
    {
        m_minimize_close = state;

        /*
         *  Tell the world the new preference
         */
        emit signalMinimizeOnCloseChange();
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
