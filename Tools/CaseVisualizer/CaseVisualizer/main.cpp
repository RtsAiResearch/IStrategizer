#include <QtGui/QApplication>
#include "CaseVisualizer.h"
#include <QStyleFactory>

int main(int argc, char *argv[])
{
	//plastique
	QApplication::setStyle("plastique");

    QApplication a(argc, argv);
    CaseVisualizer w;
    w.show();
    return a.exec();
}
