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
         * @brief setMailCount. Set the number of unread/new mails.
         *
         *  @param unread_mail   The number of unread mails.
         *  @param new_mail      The number of new mails.
         */
        void    setMailCount( int unread_mail, int new_mail );

    signals:

        /**
         * @brief signalTest1ButtonClicked. Signal the test 1 button was clicked.
         */
        void    signalTest1ButtonClicked();

        /**
         * @brief signalTest2ButtonClicked. Signal the test 2 button was clicked.
         */
        void    signalTest2ButtonClicked();

        /**
         * @brief signalTest3ButtonClicked. Signal the test 3 button was clicked.
         */
        void    signalTest3ButtonClicked();

        /**
         * @brief signalTest4ButtonClicked. Signal the test 4 button was clicked.
         */
        void    signalTest4ButtonClicked();

    public slots:

        /**
         * @brief slotDebugChange. The debug state changed.
         */
        void    slotDebugChange();

        /**
         * @brief slotMailCount. Slot for handling unread/new mail signals.
         *
         *  @param unread_mail   The number of unread mails.
         *  @param new_mail      The number of new mails.
         */
        void    slotMailCount( int unread_mail, int new_mail );

        /**
         * @brief slotHandleTest1Button. Handle a click on the test 1 button.
         */
        void    slotHandleTest1Button();

        /**
         * @brief slotHandleTest2Button. Handle a click on the test 2 button.
         */
        void    slotHandleTest2Button();

        /**
         * @brief slotHandleTest3Button. Handle a click on the test 3 button.
         */
        void    slotHandleTest3Button();

        /**
         * @brief slotHandleTest4Button. Handle a click on the test 4 button.
         */
        void    slotHandleTest4Button();

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
