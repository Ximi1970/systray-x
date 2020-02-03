#include "windowctrl-unix.h"

#ifdef Q_OS_UNIX

WindowCtrl::WindowCtrl(QObject *parent) : QObject(parent)
{

}


void WindowCtrl::slotWindowState( QString state )
{
    m_state = state;

    emit signalDebugMessage( "Win state: " + state );
}


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


#endif // Q_OS_UNIX
