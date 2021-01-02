#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

/*
 *	Local includes
 */

/*
 *	Qt includes
 */
#include <QWidget>

/*
 *	Predefines
 */
class Preferences;

/*
 *  Namespace
 */
namespace Ui {
    class DebugWidget;
}

/**
 * @brief The DebugWidget class
 */
class DebugWidget : public QWidget
{
    Q_OBJECT

    public:

        /**
         * @brief DebugWidget. Constructor.
         *
         *  @param parent    My parent.
         */
        explicit DebugWidget( Preferences* pref, QWidget* parent = nullptr );

        /**
         * @brief changeEvent. Override the change event.
         *
         *  @param event    The event.
         */
        void    changeEvent( QEvent *event );

        /**
         * @brief setUnreadMail. Set the number of unread mails.
         *
         *  @param unread    The number of unread mails.
         */
        void    setUnreadMail( int unread );

    signals:

        /**
         * @brief signalTestButton1Clicked. Signal the test button was clicked.
         */
        void    signalTest1ButtonClicked();

        /**
         * @brief signalTestButton2Clicked. Signal the test button was clicked.
         */
        void    signalTest2ButtonClicked();

        /**
         * @brief signalTestButton3Clicked. Signal the test button was clicked.
         */
        void    signalTest3ButtonClicked();

    public slots:

        /**
         * @brief slotDebugChange. The debug state changed.
         */
        void    slotDebugChange();

        /**
         * @brief slotSetUnreadMail. Slot for handling unread mail signals.
         *
         *  @param unread_mail  The number of unread mails.
         */
        void    slotUnreadMail( int unread_mail );

        /**
         * @brief slotHandleTest1Button. Handle a click on the test button.
         */
        void    slotHandleTest1Button();

        /**
         * @brief slotHandleTest2Button. Handle a click on the test button.
         */
        void    slotHandleTest2Button();

        /**
         * @brief slotHandleTest2Button. Handle a click on the test button.
         */
        void    slotHandleTest3Button();

        /**
         * @brief slotConsole. Handle console signal.
         *
         *  @param message      The message for the console.
         */
        void    slotConsole( QString message );

    private:

        /**
         * @brief m_ui. Pointer to the widget.
         */
        Ui::DebugWidget*    m_ui;

        /**
         * @brief m_pref. Pointer to ther preferences.
         */
        Preferences*    m_pref;
};

#endif // DEBUGWIDGET_H
