/*
 *	Local includes
 */
#include "systrayx.h"

/*
 *	Qt includes
 */
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SysTrayX systrayx;

    return a.exec();
}
