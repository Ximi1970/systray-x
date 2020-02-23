/*
 *  Qt includes
 */
#include <QWidget>
#include <QWindow>
#include <QString>
#include <QCoreApplication>

/*
 *  Main include
 */
#include "windowctrl.h"

/*
 *  System includes
 */
#include "container.h"
#include "preferences.h"

/*
 *  Constructor
 */
WindowCtrl::WindowCtrl( Container* container, Preferences* pref, QObject *parent ) :
#ifdef Q_OS_UNIX
    WindowCtrlUnix( parent )
#elif defined Q_OS_WIN
    WindowCtrlWin( parent )
#else
    public QObject
#endif
{
   /*
    *  Store the containwe
    */
   m_container = container;

    /*
     *  Store preferences
     */
    m_pref = pref;

    /*
     *  Initialize
     */
    m_state = "mormal";
    m_hide_minimize = m_pref->getHideOnMinimize();
    m_tb_window = nullptr;
    m_tb_container = nullptr;
}



/*
 *  Destructor
 */
WindowCtrl::~WindowCtrl()
{
    shutdown();
}

/*
 *  Handle app shutdown
 */
void    WindowCtrl::shutdown()
{
#ifdef  ENABLE_CONTAINER

    if( m_tb_window )
    {
        /*
         *  Release the Thunderbird window
         */
        m_tb_window->setParent( nullptr );
        m_tb_window->setFlags( Qt::Window );

        /*
         *  Remove the TB container
         */
        m_container->removeWidget( m_tb_container );
        m_container->hide();

        /*
         *  Cleanup
         */
        delete m_tb_container;
//        delete m_tb_window;

        emit signalConsole( QString( "Container %1").arg( reinterpret_cast<quint64>( m_tb_container ) ) );
        emit signalConsole( QString( "Window %1").arg(  reinterpret_cast<quint64>( m_tb_window ) ) );


        m_tb_container = nullptr;
        m_tb_window = nullptr;

        /*
         *  Normalize the window
         */
        foreach( quint64 win_id, getWinIds() )
        {
            normalizeWindow( win_id );
        }

        /*
         *  Make sure all events for the release are handled
         */
        QCoreApplication::processEvents();
    }

#endif
}



void    WindowCtrl::slotWindowTest1()
{
    emit signalConsole("Test 1 started");

    // Do something.

//    displayWindowElements( "- Mozilla Thunderbird" );
//    findWindow( 4313 );

//    captureWindow( "Debugging with Firefox Developer Tools - Mozilla Thunderbird" );
    captureWindow( "- Mozilla Thunderbird" );

    emit signalConsole("Test 1 done");
}


void    WindowCtrl::slotWindowTest2()
{
    emit signalConsole("Test 2 started");

    // Do something.

    shutdown();
 /*
    if( !m_tb_window->parent() )
    {
        m_tb_window->setParent( nullptr );
        m_tb_window->setFlags( Qt::Window );

        m_container->hide();
    }
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
    if( !findWindow( title ) )
    {
        return false;
    }

#ifdef ENABLE_CONTAINER

    /*
     *  Wrap Thunderbird window
     */
    m_tb_window = QWindow::fromWinId( getWinIds()[ 0 ] );
    m_tb_container = QWidget::createWindowContainer( m_tb_window );

    /*
     *  Integrate the window container
     */
    m_container->setWidget( m_tb_container );
    m_container->setGeometry( getWinGeos()[ 0 ] );
    m_container->show();

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
    if( !findWindow( title ) )
    {
        return;
    }

#ifdef ENABLE_CONTAINER

    /*
     *  Wrap Thunderbird window
     */
    m_tb_window = QWindow::fromWinId( getWinIds()[ 0 ] );
    m_tb_window->parent();

    m_tb_container = QWidget::createWindowContainer( m_tb_window );

    /*
     *  Integrate the container
     */
    m_container->setWidget( m_tb_container );
    m_container->setGeometry( getWinGeos()[ 0 ] );
    m_container->show();

#endif
}


/*
 *  Handle change in hide on minimize state
 */
void    WindowCtrl::slotHideOnMinimizeChange()
{
    m_hide_minimize = m_pref->getHideOnMinimize();
}


/*
 *  Handle change in minimize on close state
 */
void    WindowCtrl::slotMinimizeOnCloseChange()
{
    m_minimize_close = m_pref->getMinimizeOnClose();
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

#ifdef ENABLE_CONTAINER

        if( m_tb_window )
        {
            m_container->show();
        }

#else

        foreach( quint64 win_id, getWinIds() )
        {
            normalizeWindow( win_id );

            emit signalConsole("Normalize");
        }

//        emit signalWindowNormal();    // TB window control

#endif

    } else {
        m_state = "minimized";

#ifdef ENABLE_CONTAINER

        if( m_tb_window )
        {
            m_container->hide();
        }

#else

        foreach( quint64 win_id, getWinIds() )
        {
            minimizeWindow( win_id, m_hide_minimize );

            emit signalConsole("Minimize");
        }

//        emit signalWindowMinimize();  // TB window control

#endif

    }
}
