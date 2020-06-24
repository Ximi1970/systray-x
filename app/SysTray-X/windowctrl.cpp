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
    setMinimizeType( m_pref->getMinimizeType() );

    /*
     *  Get pids
     */
    m_pid = QCoreApplication::applicationPid();
    m_ppid = getPpid();

    /*
     *  Get the TB windows
     */
    findWindows( m_ppid );
}


/*
 *  Is thunderbird our parent?
 */
bool    WindowCtrl::thunderbirdStart() const
{
   return isThunderbird( getPpid() );
}


/*
 *  Test func 1
 */
void    WindowCtrl::slotWindowTest1()
{
    emit signalConsole("Test 1 started");

    // Do something.

//    signalHideDefaultIconChange( true );

//    emit signalShow();

//    emit signalConsole( QString( "Found Ppid: %1" ).arg( getPpid() ) );
//    emit signalConsole( QString( "Found XID: %1" ).arg( getWinId() ) );

//    findWindow( "- Mozilla Thunderbird" );
//    displayWindowElements( "- Mozilla Thunderbird" );
//    findWindow( 4313 );
//    displayWindowElements( getWinId() );


    findWindows( m_ppid );

    emit signalConsole("Test 1 done");
}


/*
 *  Test func 2
 */
void    WindowCtrl::slotWindowTest2()
{
    emit signalConsole("Test 2 started");

    // Do something.

//    signalHideDefaultIconChange( false );

//    emit signalHide();

//    hideWindow( getWinId(), true );

//    findWindow( m_ppid );
//    emit signalConsole( QString( "Hwnd ppid: %1" ).arg( getWinIds()[0] ) );

    emit signalConsole("Test 2 done");
}


/*
 *  Test func 3
 */
void    WindowCtrl::slotWindowTest3()
{
    emit signalConsole("Test 3 started");

    // Do something.
//    hideWindow( getWinId(), false );

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
    setMinimizeType( m_pref->getMinimizeType() );
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
void    WindowCtrl::slotWindowState( Preferences::WindowState state )
{

    emit signalConsole( QString( "State change to: %1" ).arg( state ) );

#ifdef Q_OS_UNIX

    /*
     *  Update the TB windows and states
     */
    findWindows( m_ppid );

    QList< quint64 > win_ids = getWinIds();

    /*
     *  Minimize all?
     */
    if( state == Preferences::STATE_MINIMIZED_ALL )
    {
        emit signalConsole( QString( "Minimize all" ) );

        /*
         *   Close pressed on one of the windows, minimize them all
         */
        for( int i = 0 ; i < win_ids.length() ; ++i )
        {
            minimizeWindow( win_ids.at( i ), getMinimizeType() );
        }
    }
    else
    {
        QList< Preferences::WindowState >    win_states = getWindowStates();

        for( int i = 0 ; i < win_ids.length() ; ++i )
        {
            if( win_states.at( i ) == Preferences::STATE_MINIMIZED )
            {
                emit signalConsole( QString( "Hide: %1" ).arg( win_ids.at( i ) ) );

                hideWindow( win_ids.at( i ), getMinimizeType() );
            }
            else
            {
                emit signalConsole( QString( "Unhide: %1" ).arg( win_ids.at( i ) ) );

                hideWindow( win_ids.at( i ), false );
            }
        }
    }

#else

    /*
     *  Update the TB windows and states
     */
    findWindows( m_ppid );

    /*
     *  Minimize all?
     */
    if( state == Preferences::STATE_MINIMIZED_ALL )
    {
        emit signalConsole( QString( "Minimize all" ) );

        QList< quint64 > win_ids = getWinIds();

        /*
         *   Close pressed on one of the windows, minimize them all
         */
        for( int i = 0 ; i < win_ids.length() ; ++i )
        {
            minimizeWindow( win_ids.at( i ), getMinimizeType() );
        }
    }

#endif
}


/*
 *  Handle show / hide signal
 */
void    WindowCtrl::slotShowHide()
{

    emit signalConsole( QString( "Show/Hide" ) );

    /*
     *  Update the TB windows
     */
    findWindows( m_ppid );

    /*
     *  Get the window ids
     */
    QList< quint64 > win_ids = getWinIds();
    QList< Preferences::WindowState >    win_states = getWindowStates();

    for( int i = 0 ; i < win_ids.length() ; ++i )
    {
        if( win_states.at( i ) == Preferences::STATE_MINIMIZED )
        {
            emit signalConsole( QString( "Show %1" ).arg( win_ids.at( i ) ) );

            normalizeWindow( win_ids.at( i ) );
        }
        else
        {
            emit signalConsole( QString( "Hide %1" ).arg( win_ids.at( i ) ) );

            minimizeWindow( win_ids.at( i ), getMinimizeType() );
        }
    }
}


/*
 *  Handle close signal
 */
void    WindowCtrl::slotClose()
{

    emit signalConsole( QString( "Close" ) );

    /*
     *  Update the TB windows
     */
    findWindows( m_ppid );

    /*
     *  Get the window ids
     */
    QList< quint64 > win_ids = getWinIds();

    /*
     *  Delete all
     */
    for( int i = 0 ; i < win_ids.count() ; ++i )
    {
        deleteWindow( win_ids.at( i ) );
    }
}
