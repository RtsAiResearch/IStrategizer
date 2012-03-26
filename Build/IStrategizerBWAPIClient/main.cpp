#include <QtGui/QApplication>
#include "ClientMain.h"
#include "Console.h"

int main(int argc, char *argv[])
{
	CConsole console;
	QApplication a(argc, argv);
	ClientMain w;
	w.show();
	
	SetConsoleTitleA("IStrategizer BWAPI Client");
	int ret = a.exec();
	system("pause");
	
	return ret;
}
