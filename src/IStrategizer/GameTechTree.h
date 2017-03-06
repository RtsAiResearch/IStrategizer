///> [Serializable]

#ifndef GAMETECHTREE_H
#define GAMETECHTREE_H

#include <vector>
#include <map>
#include "EngineObject.h"
#include "EngineData.h"

namespace IStrategizer
{
    enum ResearchType;
    class RtsGame;

    ///> class=GameTechTree
    class GameTechTree : public EngineObject
    {
        OBJECT_SERIALIZABLE(GameTechTree, &m_isOnline, &m_cachedAvailResearches, &m_cachedDoneResearches);

    public:
        GameTechTree() :
            m_playerId(INVALID_TID),
            m_isOnline(true)
        {}

        GameTechTree(TID playerId) :
            m_playerId(playerId),
            m_isOnline(true)
        {}

        virtual ~GameTechTree() {}
        virtual bool ResearchDone(ResearchType researchId) const;
        virtual bool ResearchAvailable(ResearchType researchId) const;
        virtual void SetOffline(RtsGame* pBelongingGame);

    private:
        ///> type=bool
        bool m_isOnline;
        ///> type=set(int)
        Serialization::SSet<ResearchType> m_cachedAvailResearches;
        ///> type=set(int)
        Serialization::SSet<ResearchType> m_cachedDoneResearches;
        TID m_playerId;
    };
}

#endif // GAMETECHTREE_H
