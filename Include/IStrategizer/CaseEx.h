///> [Serializable]
#ifndef CASEEX_H
#define CASEEX_H

#include <vector>

#ifndef USEROBJECT_H
#include "UserObject.h"
#endif

#ifndef PLANGRAPH_H
#include "PlanGraph.h"
#endif

#ifndef GOALEX_H
#include "GoalEx.h"
#endif

#ifndef SPAIR_H
#include "SPair.h"
#endif

#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif

namespace IStrategizer
{
    class GoalEx;
    class PlanGraph;
    class GameStateEx;
    class CookedPlan;

    ///> class=CaseEx
    class CaseEx : public Serialization::UserObject
    {
    public:
        CaseEx()
            : m_planGraph(nullptr),
            m_goal(nullptr),
            m_gameState(nullptr),
            m_trialCount(1),
            m_successCount(1),
            m_pPlan(nullptr) {};
        
        CaseEx(PlanGraph* p_planGraph, GoalEx* p_goal, GameStateEx* p_gameState, int p_trialCount, int p_successCount)
            : m_planGraph(p_planGraph),
            m_goal(p_goal),
            m_gameState(p_gameState),
            m_trialCount(p_trialCount),
            m_successCount(p_successCount),
            m_pPlan(nullptr) {}

        CaseEx(OlcbpPlanDigraph* pPlan, GoalEx* pGoal, GameStateEx* pGameState, int trialCount, int successCount)
            : m_planGraph(nullptr),
            m_goal(pGoal),
            m_gameState(pGameState),
            m_trialCount(trialCount),
            m_successCount(successCount),
            m_pPlan(pPlan) {}

        //PlanGraph* GetPlanGraph() const { return m_planGraph; }
        IOlcbpPlanDigraph* GetPlan() const { return m_pPlan; }
        GoalEx* Goal() const { return m_goal; }
        GameStateEx* GameState() const { return m_gameState; }
        int TrialCount() const { return m_trialCount; }
        int SuccessCount() const { return m_successCount; }
        void TrialCount(int val) { m_trialCount = val; }
        void SuccessCount(int val) { m_successCount = val; }

        OBJECT_SERIALIZABLE(CaseEx);
        OBJECT_MEMBERS(5, &m_goal, &m_gameState, &m_trialCount, &m_successCount, &m_pPlan);

    private:
        PlanGraph* m_planGraph;
        ///> type=GoalEx*
        GoalEx* m_goal;
        ///> type=GameStateEx*
        GameStateEx* m_gameState;
        ///> type=int
        int m_trialCount;
        ///> type=int
        int m_successCount;
        ///> type=AdjListDigraph(PlanStepEx*)*
        OlcbpPlanDigraph *m_pPlan;
    };
}

#endif // CASEEX_H
