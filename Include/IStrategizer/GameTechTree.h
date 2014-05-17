#ifndef GAMETECHTREE_H
#define GAMETECHTREE_H

#include <vector>
#include <map>

namespace IStrategizer
{
    enum ResearchType;

    class GameTechTree
    {
    public:
        virtual ~GameTechTree() {}

        virtual bool ResearchDone(ResearchType p_researchId) const = 0;
        virtual bool ResearchAvailable(ResearchType p_researchId) const = 0;
    };
}

#endif // GAMETECHTREE_H
