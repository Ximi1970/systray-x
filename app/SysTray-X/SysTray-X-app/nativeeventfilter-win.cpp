#include "nativeeventfilter-win.h"


/*
 *  System includes
 */
#include "windows.h"

/*
 *  Qt includes
 */
#include <QKeySequence>
#include <QMap>


/*
 *  Define the statics
 */
const QMap< Qt::Key, int > NativeEventFilterWin::m_virtual_key_map {
    { Qt::Key_Escape, VK_ESCAPE },
    { Qt::Key_Tab, VK_TAB },
    { Qt::Key_Backtab, VK_TAB },
    { Qt::Key_Backspace, VK_BACK },
    { Qt::Key_Return, VK_RETURN },
    { Qt::Key_Enter, VK_RETURN },
    { Qt::Key_Insert, VK_INSERT },
    { Qt::Key_Delete, VK_DELETE },
    { Qt::Key_Pause, VK_PAUSE },
    { Qt::Key_Print, VK_PRINT },
    //{ Qt::Key_SysReq, ... },
    { Qt::Key_Clear, VK_CLEAR },
    { Qt::Key_Home, VK_HOME },
    { Qt::Key_End, VK_END },
    { Qt::Key_Left, VK_LEFT },
    { Qt::Key_Up, VK_UP },
    { Qt::Key_Right, VK_RIGHT },
    { Qt::Key_Down, VK_DOWN },
    { Qt::Key_PageUp, VK_PRIOR },
    { Qt::Key_PageDown, VK_NEXT },
    //{Qt::Shift, ...},
    //{Qt::Control, ...},
    //{Qt::Meta, ...},
    //{Qt::Alt, ...},
    //{Qt::AltGr, ...},
    { Qt::Key_CapsLock, VK_CAPITAL },
    { Qt::Key_NumLock, VK_NUMLOCK },
    { Qt::Key_ScrollLock, VK_SCROLL },
    { Qt::Key_F1, VK_F1 },
    { Qt::Key_F2, VK_F2 },
    { Qt::Key_F3, VK_F3 },
    { Qt::Key_F4, VK_F4 },
    { Qt::Key_F5, VK_F5 },
    { Qt::Key_F6, VK_F6 },
    { Qt::Key_F7, VK_F7 },
    { Qt::Key_F8, VK_F8 },
    { Qt::Key_F9, VK_F9 },
    { Qt::Key_F10, VK_F10 },
    { Qt::Key_F11, VK_F11 },
    { Qt::Key_F12, VK_F12 },
    { Qt::Key_F13, VK_F13 },
    { Qt::Key_F14, VK_F14 },
    { Qt::Key_F15, VK_F15 },
    { Qt::Key_F16, VK_F16 },
    { Qt::Key_F17, VK_F17 },
    { Qt::Key_F18, VK_F18 },
    { Qt::Key_F19, VK_F19 },
    { Qt::Key_F20, VK_F20 },
    { Qt::Key_F21, VK_F21 },
    { Qt::Key_F22, VK_F22 },
    { Qt::Key_F23, VK_F23 },
    { Qt::Key_F24, VK_F24 },
    //{ Qt::Key_F25, ... },
    //{ Qt::Key_F26, ... },
    //{ Qt::Key_F27, ... },
    //{ Qt::Key_F28, ... },
    //{ Qt::Key_F29, ... },
    //{ Qt::Key_F30, ... },
    //{ Qt::Key_F31, ... },
    //{ Qt::Key_F32, ... },
    //{ Qt::Key_F33, ... },
    //{ Qt::Key_F34, ... },
    //{ Qt::Key_F35, ... },
    { Qt::Key_MediaNext, VK_MEDIA_NEXT_TRACK },
    { Qt::Key_MediaPrevious, VK_MEDIA_PREV_TRACK },
    { Qt::Key_MediaPlay, VK_MEDIA_PLAY_PAUSE },
    { Qt::Key_MediaStop, VK_MEDIA_STOP },
};


/*
 *  Catch the key press
 */
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
bool NativeEventFilterWin::nativeEventFilter( const QByteArray& eventType, void* message, long* result )
#else
bool NativeEventFilterWin::nativeEventFilter( const QByteArray& eventType, void* message, qintptr* result )
#endif
{
    Q_UNUSED( eventType )
    Q_UNUSED( result )

    MSG* msg = static_cast< MSG* >( message );
    if( msg->message == WM_HOTKEY )
    {
        if( msg->wParam == SHORTCUT_ID )
        {
            activated();
            return true;
        }
     }

    return false;
}


/*
 *  Connect the shortcut key
 */
bool NativeEventFilterWin::connectShortcut( QKeySequence key_seq )
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
bool NativeEventFilterWin::connectShortcut( Qt::Key key_code, Qt::KeyboardModifiers key_modifiers )
{
    m_key_code_win = 0;
    if( key_code <= 0xffff )
    {
        SHORT virtual_key = VkKeyScanW( key_code );
        if( virtual_key > -1 )
        {
            m_key_code_win = virtual_key & 0xff;
        }
    }

    if( m_key_code_win == 0 && m_virtual_key_map.contains( key_code ) )
    {
        m_key_code_win = m_virtual_key_map[ key_code ];
    }
    else
    {
        if( key_code <= 0xffff )
        {
            m_key_code_win = key_code & 0xff;
        }
    }

    /*
     *  Convert the Qt key modifiers to Windows key modifiers
     */
    m_key_modifiers_win = 0;
    if( key_modifiers & Qt::ShiftModifier )
    {
        m_key_modifiers_win |= MOD_SHIFT;
    }
    if( key_modifiers & Qt::ControlModifier )
    {
        m_key_modifiers_win |= MOD_CONTROL;
    }
    if( key_modifiers & Qt::AltModifier )
    {
        m_key_modifiers_win |= MOD_ALT;
    }
    if( key_modifiers & Qt::MetaModifier )
    {
        m_key_modifiers_win |= MOD_WIN;
    }

    /*
     *  Register the shortcut
     */
    RegisterHotKey( (HWND)NULL, SHORTCUT_ID, m_key_modifiers_win + MOD_NOREPEAT, m_key_code_win );

    return true;
}


/*
 *  Disconnect the shortcut key
 */
bool NativeEventFilterWin::disconnectShortcut()
{
    /*
     *  Unregister the shortcut
     */
    UnregisterHotKey( (HWND)NULL, SHORTCUT_ID );

    return true;
}
