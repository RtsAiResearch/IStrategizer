#ifndef ONLINEPLANEXPANSIONEXECUTIONEX_H
#define ONLINEPLANEXPANSIONEXECUTIONEX_H

#define MAX_GOAL_WAITING_TIME 10000ll
#define MAX_ACTION_WAITING_TIME 10000ll

#include <map>
#include "PlanGraphNode.h"
#include "EngineComponent.h"
#include "WorldClock.h"
#include "RtsGame.h"

using namespace std;

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

        OnlinePlanExpansionExecutionEx(RtsGame& p_RtsGame, GoalEx* p_initialGoal, CaseBasedReasonerEx* p_casedBasedReasoner);
        void Update(RtsGame& p_RtsGame, const WorldClock& p_clock);
        void NotifyMessegeSent(RtsGame& p_RtsGame, Message* p_message);
        const PlanGraphNode* PlanRoot() const { return _planRoot; }
        PlanGraphNode* PlanRoot() { return _planRoot; }
        OlcbpPlanGraph& Plan() { return _planGraph; }

    private:
        void ExpandGoal(RtsGame& p_RtsGame, PlanGraphNode* p_rootGoal, CaseEx* p_Case);
        void UpdatePlan(RtsGame& p_RtsGame, PlanGraphNode* p_rootPlanStep, const WorldClock& p_clock);

        void NotifyChildrenForParentSuccess(PlanGraphNode* p_Node);
        void MarkCaseAsTried(PlanGraphNode* p_Step, CaseEx* p_Case);
        bool IsCaseTried(PlanGraphNode* p_Step, CaseEx* p_Case);
        bool DestroyGoalPlanIfExist(PlanGraphNode* p_PlanGoalNode);
        void ConsiderReadyChildrenForUpdate(PlanGraphNode* p_Node, PlanGraphNode::Queue &p_updateQueue);
        void UpdateActionNode(RtsGame& p_RtsGame, PlanGraphNode* pCurrentNode, const WorldClock& p_clock, PlanGraphNode::Queue& p_updateQ);
        void UpdateGoalNode(RtsGame& p_RtsGame, PlanGraphNode* p_CurrentNode, const WorldClock& p_clock, PlanGraphNode::Queue& p_updateQ);

        PlanGraphNode*                        _planRoot;
        CaseBasedReasonerEx*                _caseBasedReasoner;
        std::map<PlanGraphNode*, CaseSet>    _triedCases;
        std::map<PlanGraphNode*, CaseEx*>    _cases;
        OlcbpPlanGraph _planGraph;
    };
}

#endif    // ONLINEPLANEXPANSIONEXECUTIONEX_H
