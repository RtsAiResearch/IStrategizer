#pragma warning(push, 3)
#include <QApplication>
#include <QStyleFactory>
#pragma warning(pop)

#include "CaseVisualizer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("fusion"));

    IStrategizer::CaseVisualizer w;
    w.showMaximized();
    return a.exec();
}
