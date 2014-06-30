#include <QApplication>
#include "ClientMain.h"
#include "Console.h"
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    CConsole console;
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("fusion"));
    ClientMain w;
    w.showMaximized();
    
    SetConsoleTitleA("IStrategizer BWAPI Client");
    int ret = a.exec();
    system("pause");
    
    return ret;
}
