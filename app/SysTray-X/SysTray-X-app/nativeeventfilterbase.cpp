#include "nativeeventfilterbase.h"

/*
 *	Local includes
 */
#include "shortcut.h"

/*
 *  System includes
 */

/*
 *	Qt includes
 */
#include <QCoreApplication>
#include <QAbstractEventDispatcher>


/*
 *  Constructor
 */
NativeEventFilterBase::NativeEventFilterBase()
{
    /*
     *  Install the new filter
     */
    qApp->eventDispatcher()->installNativeEventFilter( this );
}


/*
 *  Destructor
 */
NativeEventFilterBase::~NativeEventFilterBase()
{
    if( qApp && qApp->eventDispatcher() )
    {
        /*
         *  Remove the filter
         */
        qApp->eventDispatcher()->removeNativeEventFilter( this );
    }
}


/*
 *  Send the activate signal
 */
void NativeEventFilterBase::activated()
{
    emit m_shortcut->activated();
}


/*
 *  Set the shortcut key
 */
bool NativeEventFilterBase::setShortcut( Shortcut* shortcut )
{
    m_shortcut = shortcut;

    return connectShortcut( m_shortcut->getKeyCode(), m_shortcut->getKeyModifiers() );
}


/*
 *  Unset the shortcut key
 */
bool NativeEventFilterBase::unsetShortcut()
{
    m_shortcut = nullptr;

    return disconnectShortcut();
}
