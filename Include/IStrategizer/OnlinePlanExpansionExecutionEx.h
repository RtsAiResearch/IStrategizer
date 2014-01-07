#ifndef ONLINEPLANEXPANSIONEXECUTIONEX_H
#define ONLINEPLANEXPANSIONEXECUTIONEX_H

#define MAX_GOAL_WAITING_TIME 10000ll
#define MAX_ACTION_WAITING_TIME 10000ll

#include <map>
#include "PlanGraphNode.h"
using namespace std;

#ifndef ENGINECOMPONENT_H
#include "EngineComponent.h"
#endif
#include "WorldClock.h"

namespace IStrategizer
{
    enum PlayerType;
    class PlanGraph;
    class CaseEx;
    class GoalEx;
    class CaseBasedReasonerEx;

    typedef std::map<PlanGraphNode::NodeID, PlanGraphNode*> OlcbpPlanGraph;

    class OnlinePlanExpansionExecutionEx : public EngineComponent
    {
    public:
        typedef std::set<CaseEx*> CaseSet;

        OnlinePlanExpansionExecutionEx(GoalEx* p_initialGoal, CaseBasedReasonerEx* p_casedBasedReasoner);
        void Update(const WorldClock& p_clock);
        void NotifyMessegeSent(Message* p_message);
        const PlanGraphNode* PlanRoot() const { return _planRoot; }
        PlanGraphNode* PlanRoot() { return _planRoot; }
        OlcbpPlanGraph& Plan() { return _planGraph; }

    private:
        void ExpandGoal(PlanGraphNode* p_rootGoal, CaseEx* p_pCase);
        void UpdatePlan(PlanGraphNode* p_rootPlanStep, const WorldClock& p_clock);


        void NotifyChildrenForParentSuccess(PlanGraphNode* p_pNode);
        void MarkCaseAsTried(PlanGraphNode* p_pStep, CaseEx* p_pCase);
        bool IsCaseTried(PlanGraphNode* p_pStep, CaseEx* p_pCase);
        bool DestroyGoalPlanIfExist(PlanGraphNode* p_pPlanGoalNode);
        void ConsiderReadyChildrenForUpdate(PlanGraphNode* p_pNode, PlanGraphNode::Queue &p_updateQueue);
        void UpdateActionNode(PlanGraphNode* pCurrentNode, const WorldClock& p_clock, PlanGraphNode::Queue& p_updateQ);
        void UpdateGoalNode(PlanGraphNode* p_pCurrentNode, const WorldClock& p_clock, PlanGraphNode::Queue& p_updateQ);

        PlanGraphNode*                        _planRoot;
        CaseBasedReasonerEx*                _caseBasedReasoner;
        std::map<PlanGraphNode*, CaseSet>    _triedCases;
        std::map<PlanGraphNode*, CaseEx*>    _cases;
        OlcbpPlanGraph _planGraph;
    };
}

#endif    // ONLINEPLANEXPANSIONEXECUTIONEX_H