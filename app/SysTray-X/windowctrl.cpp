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
 *  Constructor
 */
#ifdef Q_OS_UNIX
WindowCtrl::WindowCtrl( QObject *parent ) : WindowCtrlUnix( parent )
#elif Q_OS_WIN
WindowCtrl::WindowCtrl( QObject *parent ) : WindowCtrlWin( parent )
#else
class WindowCtrl : public QObject
#endif
{
    /*
     *  Initialize
     */
    m_tb_container = nullptr;
}


void    WindowCtrl::slotWindowTest1()
{
    emit signalConsole("Test 1 started");

    // Do something.

//    unsigned long win_id;
//    findWindow( "Debugging with Firefox Developer Tools - Mozilla Thunderbird", win_id );
//    findWindow( "Mozilla Thunderbird", win_id );

//    findWindow( 4313 );


    captureWindow( "Debugging with Firefox Developer Tools - Mozilla Thunderbird" );


    emit signalConsole("Test 1 done");
}


void    WindowCtrl::slotWindowTest2()
{
    emit signalConsole("Test 2 started");

    // Do something.

    emit signalConsole("Test 2 done");
}


// "Debugging with Firefox Developer Tools - Mozilla Thunderbird"

bool    WindowCtrl::captureWindow( const QString& title )
{
    unsigned long WinId;
    if( !findWindow( title, WinId ) )
    {
        return false;
    }

    /*
     *  Wrap Thunderbird window
     */
    m_tb_window = QWindow::fromWinId( WinId );
    m_tb_container = QWidget::createWindowContainer( m_tb_window );

//    container->hide();

    return true;
}


/*
 *  Handle change in window state
 */
void    WindowCtrl::slotWindowState( QString state )
{
    m_state = state;

    emit signalDebugMessage( "Win state: " + state );
}


/*
 *  Handle show / hide signal
 */
void    WindowCtrl::slotShowHide()
{
    if( m_state == "minimized" )
    {
        m_state = "normal";
//        emit signalWindowNormal();

        if( m_tb_container )
        {
            m_tb_container->show();

//            m_tb_window->setParent( nullptr );

//            delete m_tb_container;
//            m_tb_container = nullptr;
        }
    } else {
        m_state = "minimized";
//        emit signalWindowMinimize();

        if( m_tb_container )
        {
            m_tb_container->hide();
        }
    }
}

