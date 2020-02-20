/*
 *  Qt includes
 */
#include <QWidget>
#include <QWindow>

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
WindowCtrl::WindowCtrl( Preferences* pref, QObject *parent ) :
#ifdef Q_OS_UNIX
    WindowCtrlUnix( parent )
#elif defined Q_OS_WIN
    WindowCtrlWin( parent )
#else
    public QObject
#endif
{
    /*
     *  Store preferences
     */
    m_pref = pref;

    /*
     *  Initialize
     */
    m_minimize_hide = m_pref->getMinimizeHide();
    m_tb_container = nullptr;
}


void    WindowCtrl::slotWindowTest1()
{
    emit signalConsole("Test 1 started");

    // Do something.

    displayWindowElements( "- Mozilla Thunderbird" );
//    findWindow( 4313 );


//    captureWindow( "Debugging with Firefox Developer Tools - Mozilla Thunderbird" );

    emit signalConsole("Test 1 done");
}


void    WindowCtrl::slotWindowTest2()
{
    emit signalConsole("Test 2 started");

    // Do something.

    /*
     *  Disconnect container?
     */
/*
    m_tb_window->setParent( nullptr );

    delete m_tb_container;
    m_tb_container = nullptr;
*/
    emit signalConsole("Test 2 done");
}


void    WindowCtrl::slotWindowTest3()
{
    emit signalConsole("Test 3 started");

    // Do something.

    emit signalConsole("Test 3 done");
}


bool    WindowCtrl::captureWindow( const QString& title )
{
    Q_UNUSED( title )

#ifdef  FF_NEET

    unsigned long WinId;
    if( !findWindow( title ) )
    {
        return false;
    }

    /*
     *  Wrap Thunderbird window
     */
    m_tb_window = QWindow::fromWinId( WinId );
    m_tb_window->parent();

    m_tb_container = QWidget::createWindowContainer( m_tb_window );

#endif

    return true;
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
 *  Handle change in minimizeHide state
 */
void    WindowCtrl::slotMinimizeHideChange()
{
    m_minimize_hide = m_pref->getMinimizeHide();
}


/*
 *  Handle change in window state
 */
void    WindowCtrl::slotWindowState( QString state )
{
    m_state = state;
}


/*
 *  Handle show / hide signal
 */
void    WindowCtrl::slotShowHide()
{
    if( m_state == "minimized" )
    {
        m_state = "normal";

        foreach( quint64 win_id, getWinIds() )
        {
            hideWindow( win_id, false );
            normalizeWindow( win_id );
        }

//        emit signalWindowNormal();    // TB window control

    } else {
        m_state = "minimized";

        foreach( quint64 win_id, getWinIds() )
        {
            if( m_minimize_hide )
            {
                hideWindow( win_id, true );
            }
            minimizeWindow( win_id );
        }

//        emit signalWindowMinimize();  // TB window control

    }
}
