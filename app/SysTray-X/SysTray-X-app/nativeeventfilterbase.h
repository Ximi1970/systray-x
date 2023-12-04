#ifndef NATIVEEVENTFILTERBASE_H
#define NATIVEEVENTFILTERBASE_H

/*
 *	Local includes
 */

/*
 *  System includes
 */

/*
 *	Qt includes
 */
#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QGlobalStatic>

/*
 *  Predefines
 */
class Shortcut;


/**
 * @brief The NativeEventFilterBase class
 */
class NativeEventFilterBase : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

    public:

        /**
         * @brief NativeEventFilterBase. Constructor, destructor.
         */
        NativeEventFilterBase();
        ~NativeEventFilterBase();

        /**
         * @brief instance. Pointer to the instance.
         *
         *  @return Pointer to the instance.
         */
        static NativeEventFilterBase* instance();

        /**
         * @brief setShortcut. Set a shortcut.
         *
         *  @param shortcut     Shortcut to set.
         */
        bool setShortcut( Shortcut* shortcut );

        /**
         * @brief unsetShortcut. Unset a shortcut.
         */
        bool unsetShortcut();

    protected:

        /**
         * @brief connectShortcut. Connect the shortcut to the system.
         *
         *  @param key_code         The key code to connect.
         *  @param key_modifiers    The modifiers to use.
         *
         * @return Succeeded?
         */
        virtual bool connectShortcut( Qt::Key key_code, Qt::KeyboardModifiers key_modifiers ) = 0;

        /**
         * @brief disconnectShortcut. Disconnect the shortcut.
         *
         * @return Succeeded?
         */
        virtual bool disconnectShortcut() = 0;

        /**
         * @brief activated. Shortcut has been activated.
         */
        void activated();

    private:

        /**
         * @brief m_shortcut. Shortcut storage.
         */
        Shortcut* m_shortcut;
};

#define SINGLETON( ClassName )  Q_GLOBAL_STATIC( ClassName, nativeEventFilterBase ) \
    NativeEventFilterBase* NativeEventFilterBase::instance()\
    {\
        return nativeEventFilterBase;\
    }

#endif // NATIVEEVENTFILTERBASE_H
