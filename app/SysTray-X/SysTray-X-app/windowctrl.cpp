#include "windowctrl.h"

/*
 *  Qt includes
 */
#include <QWidget>
#include <QWindow>
#include <QCoreApplication>

/*
 *  System includes
 */

/*
 *  Local includes
 */
#include "debug.h"
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
    setMinimizeIconType( m_pref->getMinimizeIconType() );
    setCloseType( m_pref->getCloseType() );
    m_show_hide_active = false;

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

    emit signalConsole("Test 1 done");
}


/*
 *  Test func 2
 */
void    WindowCtrl::slotWindowTest2()
{
    emit signalConsole("Test 2 started");

    // Do something.

    emit signalConsole("Test 2 done");
}


/*
 *  Test func 3
 */
void    WindowCtrl::slotWindowTest3()
{
    emit signalConsole("Test 3 started");

    // Do something.

    emit signalConsole("Test 3 done");
}


/*
 *  Test func 4
 */
void    WindowCtrl::slotWindowTest4()
{
    emit signalConsole("Test 4 started");

    // Do something.

    emit signalConsole("Test 4 done");
}

/*
 *  Handle change in minimize type change
 */
void    WindowCtrl::slotMinimizeTypeChange()
{
    setMinimizeType( m_pref->getMinimizeType() );
}

/*
 *  Handle change in minimize icon type change
 */
void    WindowCtrl::slotMinimizeIconTypeChange()
{
    setMinimizeIconType( m_pref->getMinimizeIconType() );
}

/*
 *  Handle change in close type change
 */
void    WindowCtrl::slotCloseTypeChange()
{
    setCloseType( m_pref->getCloseType() );
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
    if( m_show_hide_active )
    {
#ifdef DEBUG_DISPLAY_ACTIONS
        emit signalConsole( "State change blocked" );
#endif
        return;
    }

#ifdef DEBUG_DISPLAY_ACTIONS
    emit signalConsole( QString( "State change to: %1" ).arg( Preferences::WindowStateString.at( state ) ) );
#endif

    /*
     *  Update the TB windows and states
     */
    findWindows( m_ppid );

    /*
     *  Minimize all?
     */
    if( state == Preferences::STATE_MINIMIZED_ALL || state == Preferences::STATE_MINIMIZED_ALL_STARTUP )
    {
#ifdef DEBUG_DISPLAY_ACTIONS
        emit signalConsole( QString( "Minimize all" ) );
#endif

        QList< quint64 > win_ids = getWinIds();

        /*
         *  Minimize on startup always to the tray
         */
        TargetType targetType = TargetType::TYPE_WINDOW_TO_SYSTEMTRAY;
        if( state == Preferences::STATE_MINIMIZED_ALL )
        {

#ifdef Q_OS_UNIX

            /*
             * Update window positions
             */
            updatePositions();

#endif

            /*
             *  Minimize target on close depends on preference
             */
            Preferences::CloseType closeType = getCloseType();
            if( closeType == Preferences::PREF_MINIMIZE_ALL_WINDOWS || closeType == Preferences::PREF_MINIMIZE_MAIN_CLOSE_CHILDREN_WINDOWS )
            {
                targetType = TargetType::TYPE_WINDOW_TO_TASKBAR;
            }
        }

        /*
         *   Close pressed on one of the windows, minimize them all
         */
        for( int i = 0 ; i < win_ids.length() ; ++i )
        {
#ifdef DEBUG_DISPLAY_ACTIONS
            emit signalConsole( QString( "Window state: %1, %2" )
                                .arg( win_ids.at( i ) )
                                .arg( Preferences::WindowStateString.at( getWindowState( win_ids.at( i ) ) ) ) );
#endif

            if( targetType == TargetType::TYPE_WINDOW_TO_TASKBAR )
            {
                minimizeWindowToTaskbar( win_ids.at( i ) );
            }
            else
            {
                minimizeWindowToTray( win_ids.at( i ) );
            }
        }
    }
    else
    {
        if( state == Preferences::STATE_MINIMIZED )
        {
            Preferences::MinimizeType minimizeType = getMinimizeType();
            if( minimizeType != Preferences::PREF_DEFAULT_MINIMIZE )
            {
                QList< quint64 > win_ids = getWinIds();
                for( int i = 0 ; i < win_ids.length() ; ++i )
                {                   
                   /*
                    *  Hide the window
                    */
                    if( getWindowState( win_ids[ i ] ) == Preferences::STATE_MINIMIZED )
                    {
                        minimizeWindowToTray( win_ids.at( i ) );
                    }
                }
            }
        }
    }

#ifdef DEBUG_DISPLAY_ACTIONS
    emit signalConsole( "State change done" );
#endif
}


/*
 *  Handle show / hide signal
 */
void    WindowCtrl::slotShowHide()
{
#ifdef DEBUG_DISPLAY_ACTIONS
    emit signalConsole( "Show/Hide" );
#endif

    /*
     *  Mark action
     */
    m_show_hide_active = true;

    /*
     *  Update the TB windows
     */
    findWindows( m_ppid );

#ifdef Q_OS_WIN

    /*
     *  Restore hidden windows ( not found by findWindws() )
     */
    normalizeWindowsHidden();

#endif

#ifdef Q_OS_UNIX

    /*
     *  Update the positions
     */
    updatePositions();

#endif

    TargetType targetType = TargetType::TYPE_WINDOW_TO_SYSTEMTRAY;
    if( getMinimizeIconType() == Preferences::PREF_DEFAULT_MINIMIZE_ICON )
    {
        targetType = TargetType::TYPE_WINDOW_TO_TASKBAR;
    }

    /*
     *  Get the window ids
     */
    QList< quint64 > win_ids = getWinIds();

    for( int i = 0 ; i < win_ids.length() ; ++i )
    {
#ifdef DEBUG_DISPLAY_ACTIONS
        emit signalConsole( QString( "Window state: %1, %2" )
                            .arg( win_ids.at( i ) )
                            .arg( Preferences::WindowStateString.at( getWindowState( win_ids.at( i ) ) ) ) );
#endif

        if( getWindowState( win_ids.at( i ) ) == Preferences::STATE_MINIMIZED || getWindowState( win_ids.at( i ) ) == Preferences::STATE_DOCKED )
        {
            normalizeWindow( win_ids.at( i ) );
        }
        else
        {
            if( targetType == TargetType::TYPE_WINDOW_TO_TASKBAR )
            {
                minimizeWindowToTaskbar( win_ids.at( i ) );
            }
            else
            {
                minimizeWindowToTray( win_ids.at( i ) );
            }
        }
    }

    /*
     *  Mark action
     */
    m_show_hide_active = false;

#ifdef DEBUG_DISPLAY_ACTIONS
    emit signalConsole( "Show/Hide end" );
#endif
}


/*
 *  Handle close signal
 */
void    WindowCtrl::slotClose()
{
#ifdef DEBUG_DISPLAY_ACTIONS
    emit signalConsole( "Close" );
#endif

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


/*
 *  Handle the startup window posiions.
 */
void    WindowCtrl::slotPositions( QList< QPoint > window_positions )
{
#ifdef Q_OS_UNIX

    /*
     *  Update the TB windows
     */
    findWindows( m_ppid );

    setPositions( window_positions );

#endif
}
