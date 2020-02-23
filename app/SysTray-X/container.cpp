#include "container.h"
#include "ui_container.h"

/*
 *  Qt includes
 */
#include <QVBoxLayout>

/*
 *  Constructor
 */
Container::Container( QWidget *parent ) : QWidget( parent )
{
    /*
     *  Construct conatainer
     */
    QVBoxLayout* layout = new QVBoxLayout( this );
    setLayout( layout );

    /*
     *  Set properties
     */
    setWindowIcon( QIcon( ":/files/icons/Thunderbird.png" ) );
}


void    Container::setWidget( QWidget* widget )
{
    layout()->addWidget( widget );
}
