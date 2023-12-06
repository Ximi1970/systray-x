#include "nativeeventfilter-win.h"


/*
 *  System includes
 */

/*
 *  Qt includes
 */
#include <QKeySequence>


/*
 *  Catch the key press
 */
bool NativeEventFilterWin::nativeEventFilter( const QByteArray& eventType, void* message, long* result )
{
    Q_UNUSED( eventType )
    Q_UNUSED( message )
    Q_UNUSED( result )

    return false;
}


/*
 *  Connect the shortcut key
 */
bool NativeEventFilterWin::connectShortcut( QKeySequence key_seq )
{
    Qt::Key key_code = Qt::Key( key_seq[ 0 ] & static_cast< int >( ~Qt::KeyboardModifierMask ) );
    Qt::KeyboardModifiers key_modifiers = Qt::KeyboardModifiers( key_seq[ 0 ] & static_cast<int>( Qt::KeyboardModifierMask ) );

    return connectShortcut( key_code, key_modifiers );
}


/*
 *  Connect the shortcut key
 */
bool NativeEventFilterWin::connectShortcut( Qt::Key key_code, Qt::KeyboardModifiers key_modifiers )
{
    // Set windows callback

    return true;
}


/*
 *  Disconnect the shortcut key
 */
bool NativeEventFilterWin::disconnectShortcut()
{
    // Remove windows callback

    return true;
}
