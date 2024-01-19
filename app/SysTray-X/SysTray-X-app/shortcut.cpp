#include "shortcut.h"

/*
 *  Local includes
 */
#include "nativeeventfilterbase.h"


/*
 *  Constructor
 */
Shortcut::Shortcut( QObject *parent )
    : QObject( parent )
{
}


/*
 *  Constructor with arguments
 */
Shortcut::Shortcut( QKeySequence key_seq, QObject *parent ) :
    Shortcut( parent )
{
    setShortcut( key_seq );
}


/*
 *  Constructor with arguments
 */
Shortcut::Shortcut( Qt::Key key_code, Qt::KeyboardModifiers key_modifiers, QObject *parent ) :
    Shortcut( parent )
{
    setShortcut( key_code, key_modifiers );
}


/*
 *  Destructor
 */
Shortcut::~Shortcut()
{
    NativeEventFilterBase::instance()->unsetShortcut();
}


/*
 *  Set the shortcut
 */
bool Shortcut::setShortcut( QKeySequence key_seq )
{
    Qt::Key key_code = Qt::Key( key_seq[ 0 ] & ~Qt::KeyboardModifierMask );
    Qt::KeyboardModifiers key_modifiers = Qt::KeyboardModifiers( key_seq[ 0 ] & Qt::KeyboardModifierMask);

    return setShortcut( key_code, key_modifiers );
}


/*
 *  Set the shortcut
 */
bool Shortcut::setShortcut( Qt::Key key_code, Qt::KeyboardModifiers key_modifiers )
{
    m_key_code = key_code;
    m_key_modifiers = key_modifiers;

    return NativeEventFilterBase::instance()->setShortcut( this );
}


/*
 *  Get the stored Qt key code
 */
Qt::Key Shortcut::getKeyCode()
{
    return m_key_code;
}


/*
 *  Get the stored Qt key modifiers
 */
Qt::KeyboardModifiers Shortcut::getKeyModifiers()
{
    return m_key_modifiers;
}
