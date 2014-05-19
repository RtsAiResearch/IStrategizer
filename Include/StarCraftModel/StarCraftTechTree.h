///> [Serializable]
#ifndef STARCRAFTTECHTREE_H
#define STARCRAFTTECHTREE_H

#ifndef GAMETECHTREE_H
#include "GameTechTree.h"
#endif

#include "BWAPI.h"
#include "SSet.h"

namespace IStrategizer
{
    enum ResearchType;

    ///> class=StarCraftTechTree
    ///> parent=GameTechTree
    class StarCraftTechTree : public GameTechTree
    {
        OBJECT_SERIALIZABLE(StarCraftTechTree);
        OBJECT_MEMBERS_P(GameTechTree, 3, &m_isOnline, &m_cachedAvailResearches, &m_cachedDoneResearches);

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

