#ifndef ABSTRACTRETRIEVER_H
#define ABSTRACTRETRIEVER_H

#ifndef ENGINECOMPONENT_H
#include "EngineComponent.h"
#endif

namespace IStrategizer
{
    class GoalEx;
    class CaseEx;
    class GameStateEx;
    class AbstractRetainer;

    class AbstractRetriever : public EngineComponent
    {
    public:
        AbstractRetriever(RtsGame& p_RtsGame, AbstractRetainer* p_Retainer, const char* p_pName) : EngineComponent(p_RtsGame, p_pName), m_pRetainer(p_Retainer) {}
        virtual CaseEx* Retrieve(const GoalEx* p_goal, const GameStateEx* p_gameState) = 0;
        
    protected:
        AbstractRetainer *m_pRetainer;
    };
}

#endif
