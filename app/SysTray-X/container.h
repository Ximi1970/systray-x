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
        Container( QWidget *parent = nullptr );

        /**
         * @brief setWidget. Set the container widget.
         *
         *   @param widget      The widget.
         */
        void    setWidget( QWidget* widget );

    private:
};

#endif // CONTAINER_H
