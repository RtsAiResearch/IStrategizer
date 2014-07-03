#ifndef BOTSTATISTICS_H
#define BOTSTATISTICS_H

#include "SVector.h"
#include "GameStatistics.h"

namespace IStrategizer
{
    const std::string g_StatisticsPath = "IStrategizerEx.stat";

    struct GameEndMessageData;

    class BotStatistics
    {
    public:
        void Read();
        void Add(GameEndMessageData* pData);

        Serialization::SVector<GameStatistics*> Statistics;
    };
}

#endif // BOTSTATISTICS_H