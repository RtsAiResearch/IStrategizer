#ifndef STARCRAFTTECHTREE_H
#define STARCRAFTTECHTREE_H

#ifndef GAMETECHTREE_H
#include "GameTechTree.h"
#endif

#include "BWAPI.h"

namespace IStrategizer
{
    enum ResearchType;

    class StarCraftTechTree : public GameTechTree
    {
    public:
        StarCraftTechTree() : m_pPlayer(nullptr) {}
        StarCraftTechTree(BWAPI::Player pPlayer);

        bool ResearchAvailable(ResearchType p_researchId) const;
        bool ResearchDone(ResearchType p_researchId) const ;

    protected:
        BWAPI::Player m_pPlayer;
    };  
}
#endif // STARCRAFTTECHTREE_H

