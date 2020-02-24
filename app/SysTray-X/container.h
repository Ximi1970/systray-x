#ifndef CONTAINER_H
#define CONTAINER_H

/*
 *	Qt includes
 */
#include <QWidget>

/*
 *  Predefines
 */
class QVBoxLayout;
class Preferences;

/**
 * @brief The Container class. Class to hold the Thunderbird window.
 */
class Container : public QWidget
{
    Q_OBJECT

    public:

        /**
         * @brief Container. Constructor.
         *
         *   @param parent      My parent.
         */
        Container( Preferences* pref, QWidget *parent = nullptr );

        /**
         * @brief setWidget. Set the container widget.
         *
         *   @param widget      The widget.
         */
        void    setWidget( QWidget* widget );

        /**
         * @brief removeWidget. Remove the container widget.
         *
         *  @param widget
         */
        void    removeWidget( QWidget* widget );

    protected:

        bool eventFilter( QObject* obj, QEvent* ev );

    private:

        /**
         * @brief closeEvent. Override the close event handler.
         *
         *  @param event    The event.
         */
        void    closeEvent( QCloseEvent* event );

#ifdef  FF_NEET

        /**
         * @brief changeEvent. Override minimize event handler.
         *
         *  @param event    The event
         */
        void    changeEvent( QEvent* event );

#endif

    signals:

        /**
         * @brief signalShowHide. Signal show / hide window.
         */
        void signalShowHide();

    public slots:

        /**
         * @brief slotMinimizeOnClose. Hnadle preference minimize on close signal.
         */
        void    slotMinimizeOnClose();

    private:

        /**
         * @brief m_pref. Pointer to the preferences storage.
         */
        Preferences*    m_pref;

        /**
         * @brief m_minimize_close. Minimize on close state.
         */
        bool   m_minimize_close;
};

#endif // CONTAINER_H
