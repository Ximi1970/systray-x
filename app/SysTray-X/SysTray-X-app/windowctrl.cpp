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
 *  Handle change in window state
 */
void    WindowCtrl::slotWindowState( Preferences::WindowState state, int id )
{
    if( m_show_hide_active )
    {
#ifdef DEBUG_DISPLAY_ACTIONS
        emit signalConsole( "State change blocked" );
#endif
        return;
    }

#ifdef DEBUG_DISPLAY_ACTIONS
    emit signalConsole( QString( "State: %1" ).arg( Preferences::WindowStateString.at( state ) ) );
    emit signalConsole( QString( "Id: %1" ).arg( id ) );
#endif

    /*
     *  Update the TB windows and states
     */
    findWindows( m_ppid );

    /*
     *  Minimize/dock all?
     */
    if( state == Preferences::STATE_MINIMIZED_STARTUP || state == Preferences::STATE_DOCKED_STARTUP )
    {
#ifdef DEBUG_DISPLAY_ACTIONS
        emit signalConsole( QString( "Minimize all" ) );
#endif

        QList< quint64 > win_ids = getWinIds();

#ifdef Q_OS_UNIX

        /*
         * Update window positions
         */
        updatePositions();

#endif

        /*
         *   Minimize/dock all
         */
        for( int i = 0 ; i < win_ids.length() ; ++i )
        {
            if( state == Preferences::STATE_MINIMIZED_STARTUP )
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
        if( state == Preferences::STATE_MINIMIZED || state == Preferences::STATE_DOCKED )
        {
            Preferences::MinimizeType minimize_type = getMinimizeType();
            if( minimize_type != Preferences::PREF_DEFAULT_MINIMIZE )
            {
                QMap< int, quint64 > ref_list = getRefIds();

                if( ref_list.contains( id ) )
                {
                    /*
                     *  Hide the window
                     */
                    if( getWindowState( ref_list[ id ] ) == Preferences::STATE_MINIMIZED )
                    {
                        minimizeWindowToTray( ref_list[ id ] );
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

    TargetType target_type = TargetType::TYPE_WINDOW_TO_SYSTEMTRAY;
    if( getMinimizeIconType() == Preferences::PREF_DEFAULT_MINIMIZE_ICON )
    {
        target_type = TargetType::TYPE_WINDOW_TO_TASKBAR;
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
            if( target_type == TargetType::TYPE_WINDOW_TO_TASKBAR )
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

#ifdef Q_OS_WIN

    /*
     *  Delete all hidden windws ( not found by findWindows() )
     */
    deleteHiddenWindows();

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


/*
 *  Handle the new window id.
 */
void    WindowCtrl::slotNewWindow( int id )
{
    /*
     *  Get the windows
     */
    findWindows( m_ppid );

    /*
     *  Try to find a corresponding x11 window
     */
    identifyWindow( id );
}


/*
 *  Handle the close window id.
 */
void    WindowCtrl::slotCloseWindow( int id, bool quit )
{
    if( quit )
    {
        /*
         *  Window is closed by TB
         */
        removeRefId( id );
        return;
    }

    QMap<int, quint64> ref_list = getRefIds();
    if( ref_list.contains( id ) )
    {
        Preferences::CloseType close_type = getCloseType();
        if( close_type == Preferences::PREF_MINIMIZE_MAIN_TRAY_CLOSE_CHILDREN_WINDOWS ||
                close_type == Preferences::PREF_MINIMIZE_ALL_WINDOWS_TRAY )
        {
            minimizeWindowToTray( ref_list[ id ] );
        }
        else
        {
            minimizeWindowToTaskbar( ref_list[ id ] );
        }
    }
}
