#ifndef GAMESTATISTICS_H
#define GAMESTATISTICS_H

#include <stdio.h>
#include <string>

namespace IStrategizer
{
    #define STAT_LINE_MAX_LENGTH 1024

    class GameStatistics
    {
    public:
        GameStatistics(bool win, std::string mapName, int mapWidth, int mapHeight, int gameCycle, int casebaseSize, int score, std::string enemyRace) :
            Win(win),
            MapWidth(mapWidth),
            MapHeight(mapHeight),
            GameCycle(gameCycle),
            CasebaseSize(casebaseSize),
            MapName(mapName),
            Score(score),
            EnemyRace(enemyRace) {}

        std::string ToString()
        {
            char buffer[STAT_LINE_MAX_LENGTH];
            sprintf_s(buffer, STAT_LINE_MAX_LENGTH, "%s,%dx%d,%c,%d,%d,%d,%s\n", 
                MapName.c_str(),
                MapWidth,
                MapHeight,
                BoolChar(Win),
                GameCycle,
                CasebaseSize,
                Score,
                EnemyRace.c_str());
            return std::string(buffer);
        }

        bool Win;
        int MapWidth;
        int MapHeight;
        int GameCycle;
        int CasebaseSize;
        int Score;
        std::string MapName;
        std::string EnemyRace;

    private:
        char BoolChar(bool val) { return val ? '1' : '0'; }
    };
}

#endif // GAMESTATISTICS_H