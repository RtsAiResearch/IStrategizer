#ifndef ABSTRACTRETRIEVER_H
#define ABSTRACTRETRIEVER_H

#ifndef ENGINECOMPONENT_H
#include "EngineComponent.h"
#endif

class GoalEx;
class CaseEx;
class GameStateEx;

namespace OLCBP
{
	class AbstractRetainer;

    class AbstractRetriever : public EngineComponent
    {
    public:
                        AbstractRetriever(AbstractRetainer* p_pRetainer, const char* p_pName) : EngineComponent(p_pName), m_pRetainer(p_pRetainer) {}
        virtual CaseEx* Retrieve(const GoalEx* p_goal, const GameStateEx* p_gameState) = 0;
		
	protected:
		AbstractRetainer *m_pRetainer;

    };
}

#endif