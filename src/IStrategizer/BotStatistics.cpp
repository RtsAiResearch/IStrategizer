#include "BotStatistics.h"
#include "DataMessage.h"
#include <fstream>

using namespace IStrategizer;
using namespace std;

void BotStatistics::Add(GameEndMessageData* pData)
{
    GameStatistics stats(pData->IsWinner, false, pData->Score);

    fstream pen;
    pen.open(g_StatisticsPath, ios::out | ofstream::app);
    pen << stats.ToString();
    pen.flush();
    pen.close();
}