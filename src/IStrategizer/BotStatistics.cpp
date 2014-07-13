#include "BotStatistics.h"
#include "DataMessage.h"
#include "GameStatistics.h"
#include <fstream>

using namespace IStrategizer;
using namespace std;

void BotStatistics::Add(GameStatistics gameStatistics)
{
    fstream pen;
    pen.open(g_StatisticsPath, ios::out | ofstream::app);
    pen << gameStatistics.ToString();
    pen.flush();
    pen.close();
}