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
        GameStatistics(bool win = false, bool draw = false, int score = 0) :
            Win(win),
            Draw(draw),
            Score(score) {}

        std::string ToString()
        {
            char buffer[STAT_LINE_MAX_LENGTH];
            sprintf_s(buffer, STAT_LINE_MAX_LENGTH, "%c %c %d\n",
               BoolChar(Win),
                BoolChar(Draw),
                Score);
            return std::string(buffer);
        }

        bool Win;
        bool Draw;
        int Score;

    private:
        char BoolChar(bool val) { return val ? 'T' : 'F'; }
    };
}

#endif // GAMESTATISTICS_H