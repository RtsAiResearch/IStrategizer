#ifndef BOTSTATISTICS_H
#define BOTSTATISTICS_H

#include "GameStatistics.h"
#include <vector>

namespace IStrategizer
{
    const std::string g_StatisticsPath = "IStrategizerEx.stat";

    struct GameEndMessageData;

    class BotStatistics
    {
    public:
        void Add(GameEndMessageData* pData);

        std::vector<GameStatistics> Statistics;
    };
}

#endif // BOTSTATISTICS_H