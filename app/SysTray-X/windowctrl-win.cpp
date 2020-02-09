#include "windowctrl-win.h"

#ifdef Q_OS_WIN

/*
 *  Constructor
 */
WindowCtrl::WindowCtrl(QObject *parent) : QObject(parent)
{

}


/*
 *  Handle change in window state
 */
void WindowCtrl::slotWindowState( QString state )
{
    m_state = state;

    emit signalDebugMessage( "Win state: " + state );
}


/*
 *  Handle show / hide signal
 */
void WindowCtrl::slotShowHide()
{
    if( m_state == "minimized" )
    {
        m_state = "normal";
        emit signalWindowNormal();
    } else {
        m_state = "minimized";
        emit signalWindowMinimize();
    }
}

#endif // Q_OS_WIN
