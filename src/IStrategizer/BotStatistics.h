#ifndef BOTSTATISTICS_H
#define BOTSTATISTICS_H

#include <vector>

namespace IStrategizer
{
    const std::string g_StatisticsPath = "IStrategizerEx.stat";
    
    class GameStatistics;

    class BotStatistics
    {
    public:
        void Add(GameStatistics gameStatistics);
    };
}

#endif // BOTSTATISTICS_H