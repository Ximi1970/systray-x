#include "nativeeventfilter-x11.h"


/*
 *  System includes
 */
#include <xcb/xcb.h>

/*
 *  Qt includes
 */
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QX11Info>
#endif
#include <QKeySequence>


/*
 *  Define the statics
 */
const QVector< int > NativeEventFilterX11::m_special_modifiers = { 0, Mod2Mask, LockMask, ( Mod2Mask | LockMask ) };
const int NativeEventFilterX11::m_valid_mods_mask = ShiftMask | ControlMask | Mod1Mask | Mod4Mask;

/*
 *  Catch the key press
 */

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
bool NativeEventFilterX11::nativeEventFilter( const QByteArray& eventType, void* message, long* result )
#else
bool NativeEventFilterX11::nativeEventFilter( const QByteArray& eventType, void* message, qintptr* result )
#endif
{
    Q_UNUSED( eventType )
    Q_UNUSED( result )

    xcb_generic_event_t* generic_event = static_cast< xcb_generic_event_t* >( message );
    if( generic_event->response_type == XCB_KEY_PRESS )
    {
        xcb_key_press_event_t key_press_event = *static_cast< xcb_key_press_event_t* >( message );

        if( m_key_code_x11 == key_press_event.detail &&
                m_key_modifiers_x11 == ( key_press_event.state & NativeEventFilterX11::m_valid_mods_mask ) )
        {
            activated();
        }
    }

    return false;
}


/*
 *  Connect the shortcut key
 */
bool NativeEventFilterX11::connectShortcut( QKeySequence key_seq )
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Qt::Key key_code = Qt::Key( key_seq[ 0 ] & static_cast< int >( ~Qt::KeyboardModifierMask ) );
    Qt::KeyboardModifiers key_modifiers = Qt::KeyboardModifiers( key_seq[ 0 ] & static_cast<int>( Qt::KeyboardModifierMask ) );
#else
    Qt::Key key_code = Qt::Key( key_seq[ 0 ].toCombined() & static_cast< int >( ~Qt::KeyboardModifierMask ) );
    Qt::KeyboardModifiers key_modifiers = Qt::KeyboardModifiers( key_seq[ 0 ].toCombined() & static_cast<int>( Qt::KeyboardModifierMask ) );
#endif
    return connectShortcut( key_code, key_modifiers );
}


/*
 *  Connect the shortcut key
 */
bool NativeEventFilterX11::connectShortcut( Qt::Key key_code, Qt::KeyboardModifiers key_modifiers )
{
    /*
     *  Convert the Qt key code to a X11 key code
     */
    QString key_string;
    switch( key_code )
    {
        case Qt::Key_MediaLast :
        case Qt::Key_MediaPrevious :
            key_string = "XF86AudioPrev";
            break;
        case Qt::Key_MediaNext :
            key_string = "XF86AudioNext";
            break;
        case Qt::Key_MediaPause :
        case Qt::Key_MediaPlay :
        case Qt::Key_MediaTogglePlayPause :
            key_string = "XF86AudioPlay";
            break;
        case Qt::Key_MediaRecord :
            key_string = "XF86AudioRecord";
            break;
        case Qt::Key_MediaStop :
            key_string = "XF86AudioStop";
            break;
        default :
            key_string = QKeySequence( key_code ).toString();
    }

    KeySym key_sym = XStringToKeysym( key_string.toLocal8Bit().constData() );
    if( key_sym == NoSymbol )
    {
        if( key_code <= 0xffff )
        {
            //  Bad conversion, use only the key
            key_sym = key_code;
        }
        else
        {
            //  Cannot decode the key
            return false;
        }
    }

    /*
     *  Convert the Qt key modifiers to X11 key modifiers
     */
    m_key_modifiers_x11 = 0;
    if( key_modifiers & Qt::ShiftModifier )
    {
        m_key_modifiers_x11 |= ShiftMask;
    }
    if( key_modifiers & Qt::ControlModifier )
    {
        m_key_modifiers_x11 |= ControlMask;
    }
    if( key_modifiers & Qt::AltModifier )
    {
        m_key_modifiers_x11 |= Mod1Mask;
    }
    if( key_modifiers & Qt::MetaModifier )
    {
        m_key_modifiers_x11 |= Mod4Mask;
    }

    /*
     *  Get the X11 display
     */
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Display *display = QX11Info::display();
#else
    QNativeInterface::QX11Application *x11App = qApp->nativeInterface<QNativeInterface::QX11Application>();
    Display *display = x11App->display();
#endif

    /*
     *  Get the final key code
     */
    m_key_code_x11 = XKeysymToKeycode( display, key_sym );

    /*
     *  Grab the key
     */
    for( int special_mod : NativeEventFilterX11::m_special_modifiers )
    {
        XGrabKey( display, m_key_code_x11, m_key_modifiers_x11 | special_mod, XDefaultRootWindow( display ), True, GrabModeAsync, GrabModeAsync );
    }

    XSync( display, False );

    return true;
}


/*
 *  Disconnect the shortcut key
 */
bool NativeEventFilterX11::disconnectShortcut()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Display *display = QX11Info::display();
#else
    QNativeInterface::QX11Application *x11App = qApp->nativeInterface<QNativeInterface::QX11Application>();
    Display *display = x11App->display();
#endif

    /*
     *  Ungrab the key
     */
    for( int special_mod : NativeEventFilterX11::m_special_modifiers )
    {
        XUngrabKey( display, m_key_code_x11, m_key_modifiers_x11 | special_mod, XDefaultRootWindow( display ) );
    }

    XSync( display, False );

    return true;
}
