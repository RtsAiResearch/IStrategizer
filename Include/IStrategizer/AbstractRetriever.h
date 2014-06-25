#ifndef ABSTRACTRETRIEVER_H
#define ABSTRACTRETRIEVER_H

#include "EngineComponent.h"
#include "EngineData.h"
#include <set>

namespace IStrategizer
{
    class GoalEx;
    class CaseEx;
    class RtsGame;
    class AbstractRetainer;

    class AbstractRetriever : public EngineComponent
    {
    public:
        struct RetrieveOptions
        {
            GoalType GoalTypeId;
            
            PlanStepParameters Parameters;

            RtsGame* pGameState;

            std::set<CaseEx*> Exclusions;
        };

        AbstractRetriever(AbstractRetainer* p_pRetainer, const char* p_pName) : EngineComponent(p_pName), m_pRetainer(p_pRetainer) {}
        virtual CaseEx* Retrieve(RetrieveOptions options) = 0;
        
    protected:
        AbstractRetainer *m_pRetainer;
    };
}

#endif
