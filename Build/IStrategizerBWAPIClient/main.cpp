#include <QApplication>
#include <QStyleFactory>
#include "ClientMain.h"
#include "Console.h"
#include "StarCraftGame.h"

using namespace StarCraftModel;

int main(int argc, char *argv[])
{
    CConsole console;
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("fusion"));

    StarCraftGame* game = new StarCraftGame;
    ClientMain window (game);
    window.show();
    
    SetConsoleTitleA("IStrategizer BWAPI Client");
    int ret = a.exec();
    system("pause");
    
    delete game;
    return ret;
}
