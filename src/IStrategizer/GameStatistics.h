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
        GameStatistics(bool win, std::string mapName, int mapWidth, int mapHeight, int gameCycle, int casebaseSize, int score) :
            Win(win),
            MapWidth(mapWidth),
            MapHeight(mapHeight),
            GameCycle(gameCycle),
            CasebaseSize(casebaseSize),
            MapName(mapName),
            Score(score) {}

        std::string ToString()
        {
            char buffer[STAT_LINE_MAX_LENGTH];
            sprintf_s(buffer, STAT_LINE_MAX_LENGTH, "%s,%dx%d,%c,%d,%d,%d\n", 
                MapName.c_str(),
                MapWidth,
                MapHeight,
                BoolChar(Win),
                GameCycle,
                CasebaseSize,
                Score);
            return std::string(buffer);
        }

        bool Win;
        int MapWidth;
        int MapHeight;
        int GameCycle;
        int CasebaseSize;
        int Score;
        std::string MapName;


    private:
        char BoolChar(bool val) { return val ? '1' : '0'; }
    };
}

#endif // GAMESTATISTICS_H