#include "nativeeventfilter-win.h"


/*
 *  System includes
 */
#include "windows.h"

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
    Q_UNUSED( result )

    MSG* msg = reinterpret_cast< MSG* >( message );

    if( msg->message == WM_HOTKEY )
    {
        if( msg->wParam == 100 )
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
    Qt::Key key_code = Qt::Key( key_seq[ 0 ] & static_cast< int >( ~Qt::KeyboardModifierMask ) );
    Qt::KeyboardModifiers key_modifiers = Qt::KeyboardModifiers( key_seq[ 0 ] & static_cast<int>( Qt::KeyboardModifierMask ) );

    return connectShortcut( key_code, key_modifiers );
}


/*
 *  Connect the shortcut key
 */
bool NativeEventFilterWin::connectShortcut( Qt::Key key_code, Qt::KeyboardModifiers key_modifiers )
{
    //  https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerhotkey


    // Set windows callback
    RegisterHotKey( (HWND)MainWindow::winId(),        // Set the system identifier of the widget window that will handle the HotKey
                        100,                         // Set identifier HotKey
                        MOD_ALT | MOD_SHIFT,         // Set modifiers
                        'D' );                       // We define hotkeys for HotKey

    return true;
}


/*
 *  Disconnect the shortcut key
 */
bool NativeEventFilterWin::disconnectShortcut()
{
    // Remove windows callback
    UnregisterHotKey( (HWND)MainWindow::winId(), 100 );

    return true;
}
