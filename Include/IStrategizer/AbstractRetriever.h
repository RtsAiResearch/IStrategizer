#ifndef ABSTRACTRETRIEVER_H
#define ABSTRACTRETRIEVER_H

#ifndef ENGINECOMPONENT_H
#include "EngineComponent.h"
#endif
#include <set>

namespace IStrategizer
{
    class GoalEx;
    class CaseEx;
    class GameStateEx;
    class AbstractRetainer;

    class AbstractRetriever : public EngineComponent
    {
    public:
        AbstractRetriever(AbstractRetainer* p_pRetainer, const char* p_pName) : EngineComponent(p_pName), m_pRetainer(p_pRetainer) {}
        virtual CaseEx* Retrieve(const GoalEx* pGoal, const GameStateEx* pGameState, const std::set<CaseEx*>& exclusion) = 0;
        
    protected:
        AbstractRetainer *m_pRetainer;
    };
}

#endif
