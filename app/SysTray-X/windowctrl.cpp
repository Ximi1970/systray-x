/*
 *  Qt includes
 */
#include <QWidget>
#include <QWindow>
#include <QCoreApplication>

/*
 *  Main include
 */
#include "windowctrl.h"

/*
 *  System includes
 */
#include "preferences.h"

/*
 *  Constructor
 */
WindowCtrl::WindowCtrl( Preferences* pref, QObject *parent )
#ifdef Q_OS_UNIX
    : WindowCtrlUnix( parent )
#elif defined Q_OS_WIN
    : WindowCtrlWin( parent )
#else
    : public QObject
#endif
{
    /*
     *  Store preferences
     */
    m_pref = pref;

    /*
     *  Initialize
     */
    m_minimize_type = m_pref->getMinimizeType();
    m_hide_minimize = m_pref->getHideOnMinimize();

    /*
     *  Get pids
     */
    m_pid = QCoreApplication::applicationPid();
    m_ppid = getPpid();

    /*
     *  Get the TB window
     */
    findWindow( m_ppid );
}


void    WindowCtrl::slotWindowTest1()
{
    emit signalConsole("Test 1 started");

    // Do something.
//    emit signalConsole( QString( "Found Ppid: %1" ).arg( getPpid() ) );
//    emit signalConsole( QString( "Found XID: %1" ).arg( getWinId() ) );

//    findWindow( "- Mozilla Thunderbird" );
//    displayWindowElements( "- Mozilla Thunderbird" );
//    findWindow( 4313 );
//    displayWindowElements( getWinId() );

    emit signalConsole("Test 1 done");
}


void    WindowCtrl::slotWindowTest2()
{
    emit signalConsole("Test 2 started");

    // Do something.

//    findWindow( m_ppid );
//    emit signalConsole( QString( "Hwnd ppid: %1" ).arg( getWinIds()[0] ) );

    hideWindow( getWinId(), true );

    emit signalConsole("Test 2 done");
}


void    WindowCtrl::slotWindowTest3()
{
    emit signalConsole("Test 3 started");

    // Do something.
    hideWindow( getWinId(), false );

//    emit signalConsole( QString( "Pid %1" ).arg( m_pid ) );
//    emit signalConsole( QString( "Ppid %1" ).arg( m_ppid ) );

    emit signalConsole("Test 3 done");
}


/*
 *  Handle window title signal
 */
void    WindowCtrl::slotWindowTitle( QString title )
{
    /*
     *  Store the window title
     */
    m_window_title = title;

    /*
     *  Get the window IDs
     */
    findWindow( title );
}


/*
 *  Handle change in minimize type change
 */
void    WindowCtrl::slotMinimizeTypeChange()
{
    m_minimize_type = m_pref->getMinimizeType();
}


/*
 *  Handle change in hide on minimize state
 */
void    WindowCtrl::slotHideOnMinimizeChange()
{
    m_hide_minimize = m_pref->getHideOnMinimize();
}


/*
 *  Handle change in start minimized state
 */
void    WindowCtrl::slotStartMinimizedChange()
{
    m_start_minimized = m_pref->getStartMinimized();
}


/*
 *  Handle change in window state
 */
void    WindowCtrl::slotWindowState( QString state )
{
    if( m_state != state )
    {
        m_state = state;

        if( state == "normal" )
        {
            hideWindow( getWinId(), false );
        }
        else
        {
            hideWindow( getWinId(), m_hide_minimize );
        }
    }
}


/*
 *  Handle show / hide signal
 */
void    WindowCtrl::slotShowHide()
{
    if( m_state == "minimized" )
    {
        m_state = "normal";
        normalizeWindow( getWinId() );
    }
    else
    {
        m_state = "minimized";
        minimizeWindow( getWinId(), m_minimize_type );
//        minimizeWindow( getWinId(), m_hide_minimize );
    }
}


/*
 *  Handle close signal
 */
void    WindowCtrl::slotClose()
{
    deleteWindow( getWinId() );
}
