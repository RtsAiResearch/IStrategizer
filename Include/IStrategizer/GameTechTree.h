///> [Serializable]

#ifndef GAMETECHTREE_H
#define GAMETECHTREE_H

#include <vector>
#include <map>
#include "UserObject.h"

namespace IStrategizer
{
    enum ResearchType;
    class RtsGame;

    ///> class=GameTechTree
    class GameTechTree : public Serialization::UserObject
    {
        OBJECT_MEMBERS(1, &m_isOnline);

    public:
        GameTechTree() :
            m_isOnline(true)
        {}

        virtual ~GameTechTree() {}
        virtual bool ResearchDone(ResearchType researchId) const = 0;
        virtual bool ResearchAvailable(ResearchType researchId) const = 0;
        virtual void SetOffline(RtsGame* pBelongingGame) = 0;
        bool IsOnline() const { return m_isOnline; }

    protected:
        ///> type=bool
        bool m_isOnline;
    };
}

#endif // GAMETECHTREE_H
