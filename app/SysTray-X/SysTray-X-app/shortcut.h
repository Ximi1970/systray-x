#ifndef SHORTCUT_H
#define SHORTCUT_H

/*
 *	Local includes
 */
#include "nativeeventfilterbase.h"

/*
 *	Qt includes
 */
#include <QObject>
#include <QKeySequence>


/**
 * @brief The Shortcut class
 */
class Shortcut : public QObject
{
    Q_OBJECT

    private:

        /**
         * @brief NativeEventFilterBase. Singleton friend class.
         */
        friend class NativeEventFilterBase;

    public:

        /**
         * @brief Shortcut. Constructor, destructor.
         *
         *  @param parent   Parent object.
         */
        explicit Shortcut( QObject* parent = nullptr );
        ~Shortcut() override;

        /**
         * @brief Shortcut Constructor with arguments.
         *
         *  @param key_seq  The Qt key sequence for the shortcut.
         *  @param parent   The parent object.
         */
        explicit Shortcut( QKeySequence key_seq, QObject* parent = nullptr );

        /**
         * @brief Shortcut. Constructor with arguments.
         *
         *  @param keyCode      The Qt key code for the shortcut.
         *  @param modifiers    The Qt key modifiers for the shortcut.
         *  @param parent       The parent object.
         */
        explicit Shortcut( Qt::Key keyCode, Qt::KeyboardModifiers modifiers, QObject* parent = nullptr );

        /**
         * @brief getKeyCode. Get the stored key code.
         *
         * @return  The key code.
         */
        Qt::Key getKeyCode();

        /**
         * @brief getKeyModifiers. Get the stored key modifiers.
         *
         * @return  The key modifiers.
         */
        Qt::KeyboardModifiers getKeyModifiers();

    private:

        /**
         * @brief setShortcut. Set the shortcut.
         *
         *  @param key_seq  The Qt keysequence to use.
         *
         * @return  Succeeded?
         */
        bool setShortcut( QKeySequence key_seq );

        /**
         * @brief setShortcut. Set the shortcut.
         *
         *  @param key_code         The Qt key code to use.
         *  @param key_modifiers    The Qt key modifiers to use.
         *
         * @return  Succeeded?
         */
        bool setShortcut( Qt::Key key_code, Qt::KeyboardModifiers key_modifiers );

    signals:

        /**
         * @brief activated. Signal send when activated.
         */
        void activated();

    private:

        /**
         * @brief m_key_code. Storage for the key code.
         */
        Qt::Key m_key_code;

        /**
         * @brief m_key_modifiers. Storage for the key modifiers.
         */
        Qt::KeyboardModifiers m_key_modifiers;
};

#endif // SHORTCUT_H
