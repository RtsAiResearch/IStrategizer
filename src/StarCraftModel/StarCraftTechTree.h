///> [Serializable]
#ifndef STARCRAFTTECHTREE_H
#define STARCRAFTTECHTREE_H

#include "GameTechTree.h"
#include "BWAPI.h"
#include "SSet.h"

namespace IStrategizer
{
    enum ResearchType;

    ///> class=StarCraftTechTree
    ///> parent=GameTechTree
    class StarCraftTechTree : public GameTechTree
    {
		OBJECT_SERIALIZABLE_P(StarCraftTechTree, GameTechTree, &m_isOnline, &m_cachedAvailResearches, &m_cachedDoneResearches);
    public:
        StarCraftTechTree() : m_isOnline(true), m_pPlayer(nullptr) {}
        StarCraftTechTree(BWAPI::Player pPlayer) :
            m_isOnline(true),
            m_pPlayer(pPlayer)
        {}

        bool ResearchAvailable(ResearchType p_researchId) const;
        bool ResearchDone(ResearchType p_researchId) const ;
        void SetOffline(RtsGame* pBelongingGame);

    protected:
        BWAPI::Player m_pPlayer;

        ///> type=bool
        bool m_isOnline;
        ///> type=set(int)
        Serialization::SSet<ResearchType> m_cachedAvailResearches;
        ///> type=set(int)
        Serialization::SSet<ResearchType> m_cachedDoneResearches;
    };  
}
#endif // STARCRAFTTECHTREE_H

