#ifndef BOTSTATISTICS_H
#define BOTSTATISTICS_H

#include "IStrategizerEx.h"

namespace IStrategizer
{
    class GameStatistics;

    class BotStatistics
    {
    public:
        void Add(GameStatistics gameStatistics);
    };
}

#endif // BOTSTATISTICS_H