#ifndef NATIVEEVENTFILTERX11_H
#define NATIVEEVENTFILTERX11_H

/*
 *	Local includes
 */
#include "nativeeventfilterbase.h"

/*
 *  System includes
 */
#include <X11/Xlib.h>

/*
 *	Qt includes
 */


/**
 * @brief The NativeEventFilterX11 class
 */
class NativeEventFilterX11 : public NativeEventFilterBase
{
    public:

        /**
         * @brief nativeEventFilter. Set the native event filter to catch the key press.
         *
         *  @param eventType    Event type to handle.
         *  @param message      Message to handle.
         *  @param result       Result to handle.
         *
         * @return Result
         */
        bool nativeEventFilter( const QByteArray& eventType, void* message, long* result ) override;

    protected:

        /**
         * @brief connectShortcut. Connect the shortcut to the system.
         *
         *  @param key_seq     Key sequence to connect.
         *
         * @return Succeeded?
         */
        bool connectShortcut( QKeySequence key_seq );

        /**
         * @brief connectShortcut. Connect the shortcut to the system.
         *
         *  @param key_code         Qt key code to connect.
         *  @param key_modifiers    Qt key modifiers to connect.
         *
         * @return Succeeded?
         */
        bool connectShortcut( Qt::Key key_code, Qt::KeyboardModifiers key_modifiers );

        /**
         * @brief disconnectShortcut. Disconnect the shortcut.
         *
         * @return Succeeded?
         */
        bool disconnectShortcut();

    private:

        /**
         * @brief m_key_modifiers_x11. Storage for the X11 key modifiers.
         */
        int m_key_modifiers_x11;

        /**
         * @brief m_key_code_x11. Storage for the X11 key code.
         */
        KeyCode m_key_code_x11;

        /**
         * @brief m_special_modifiers. Storage for the special modifiers.
         */
        static const QVector< int > m_special_modifiers;

        /**
         * @brief m_valid_mods_mask. Storage for the valid modifiers mask.
         */
        static const int m_valid_mods_mask;
};

SINGLETON( NativeEventFilterX11 )

#endif // NATIVEEVENTFILTERX11_H
