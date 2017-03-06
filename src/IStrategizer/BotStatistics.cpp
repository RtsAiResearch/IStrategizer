#include "BotStatistics.h"
#include "DataMessage.h"
#include "GameStatistics.h"
#include <fstream>

using namespace IStrategizer;
using namespace std;

void BotStatistics::Add(GameStatistics gameStatistics)
{
    // FIXME: If read and write dirs are diff this will be a bug
    fstream pen;
    pen.open(ENGINE_IO_WRITE_DIR, ios::out | ofstream::app);
    pen << gameStatistics.ToString();
    pen.flush();
    pen.close();
}