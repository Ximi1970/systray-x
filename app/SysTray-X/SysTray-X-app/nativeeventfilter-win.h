#ifndef NATIVEEVENTFILTERWIN_H
#define NATIVEEVENTFILTERWIN_H

/*
 *	Local includes
 */
#include "nativeeventfilterbase.h"

/*
 *  System includes
 */

/*
 *	Qt includes
 */



/*
 *  Defines
 */
#define SHORTCUT_ID 1024


/**
 * @brief The NativeEventFilterWin class
 */
class NativeEventFilterWin : public NativeEventFilterBase
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
         * @brief m_key_modifiers_win. Storage for the X11 key modifiers.
         */
        int m_key_modifiers_win;

        /**
         * @brief m_key_code_win. Storage for the X11 key code.
         */
        int m_key_code_win;

        /**
         * @brief m_virtual_key_map. Holds the Qt to Windows virtual key translation map.
         */
        static const QMap< Qt::Key, int > m_virtual_key_map;
};

SINGLETON( NativeEventFilterWin )

#endif // NATIVEEVENTFILTERWIN_H
