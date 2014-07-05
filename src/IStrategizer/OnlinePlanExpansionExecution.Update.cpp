#include "OnlinePlanExpansionExecution.h"
#include "CaseBasedReasonerEx.h"
#include "WinGameGoal.h"
#include "CaseEx.h"
#include "RtsGame.h"
#include "Action.h"
#include "MessagePump.h"
#include "Message.h"
#include <crtdbg.h>
#include "AbstractReviser.h"
#include "AbstractRetriever.h"
#include "GamePlayer.h"
#include "Toolbox.h"
#include "Logger.h"
#include "RetainerEx.h"
#include "DataMessage.h"
#include "GoalFactory.h"

using namespace std;
using namespace IStrategizer;

//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::Update(_In_ const WorldClock& clock)
{
    m_pOlcbpPlan->Lock();

    // We have exhausted all possible plans. We have surrendered, nothing to do
    if (m_pOlcbpPlan->Size() > 0)
    {
        IOlcbpPlan::NodeQueue actionQ;
        IOlcbpPlan::NodeQueue goalQ;
        typedef unsigned GoalKey;
        map<GoalKey, IOlcbpPlan::NodeID> goalTable;

        // 1st pass: get ready nodes only
        GetReachableReadyNodes(actionQ, goalQ);

        // 2nd pass: prioritize and filter goal updates
        while (!goalQ.empty())
        {
            GoalEx* pCurrGoal = (GoalEx*)m_pOlcbpPlan->GetNode(goalQ.front());
            GoalKey typeKey = pCurrGoal->Key();

            if (!IsNodeDone(goalQ.front()))
            {
                goalTable[typeKey] = goalQ.front();
            }

            goalQ.pop();
        }

        // 3rd pass: actual node update
        for (auto goalEntry : goalTable)
        {
            // Only update a goal node if it still exist
            // It is normal that a previous updated goal node during this pass 
            // failed and its snippet was destroyed, and as a result a node that
            // was considered for update does not exist anymore
            if (m_pOlcbpPlan->Contains(goalEntry.second))
                UpdateGoalNode(goalEntry.second, clock);
        }

        while (!actionQ.empty())
        {
            // Only update an action node if it still exist
            // What applies to a goal in the 3rd pass apply here
            if (m_pOlcbpPlan->Contains(actionQ.front()))
                UpdateActionNode(actionQ.front(), clock);

            actionQ.pop();
        }
    }

    m_pOlcbpPlan->Unlock();

    if (m_planStructureChangedThisFrame)
    {
        g_MessagePump.Send(new DataMessage<IOlcbpPlan>(0, MSG_PlanStructureChange, nullptr));
        m_planStructureChangedThisFrame = false;
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::UpdateGoalNode(_In_ IOlcbpPlan::NodeID currentNode, _In_ const WorldClock& clock)
{
    GoalEx* pCurrentGoalNode = (GoalEx*)m_pOlcbpPlan->GetNode(currentNode);

    // fast return if node state reached a final state (i.e succeeded or failed)
    _ASSERTE(!IsNodeDone(currentNode));

#pragma region Node Open
    if (IsNodeOpen(currentNode))
    {
        _ASSERTE(pCurrentGoalNode->State() == ESTATE_NotPrepared);
        _ASSERTE(IsNodeDone(currentNode) == false);

        bool hasPreviousPlan = DestroyGoalSnippetIfExist(currentNode);

        // The goal was previously expanded with a plan, but it somehow failed
        // Thats why it is now open
        // Revise the node belonging case as failed case
        if (hasPreviousPlan)
        {
            LogInfo("Node with plan-step '%s' is open and has children nodes, case is sent for revision and children have been destroyed", pCurrentGoalNode->ToString().c_str());

            CaseEx* currentCase = GetLastCaseForGoalNode(currentNode);
            m_pCbReasoner->Reviser()->Revise(currentCase, false);
            UpdateHistory(currentCase);
            m_pCbReasoner->Retainer()->Retain(currentCase);
        }

        if (pCurrentGoalNode->SuccessConditionsSatisfied(*g_Game))
        {
            LogInfo("Goal %s already satisfied, no need to expand it, closing the node", pCurrentGoalNode->ToString().c_str());
            pCurrentGoalNode->State(ESTATE_Succeeded, *g_Game, clock);
            CloseNode(currentNode);
            OnGoalNodeSucceeded(currentNode);
        }
        else
        {
            IOlcbpPlan::NodeID satisfyingGoalNode = GetNodeData(currentNode).SatisfyingGoal;
            AbstractRetriever::RetrieveOptions options;
            options.ExcludedCases = GetNodeData(currentNode).TriedCases;

            if (satisfyingGoalNode != IOlcbpPlan::NullNodeID)
            {
                LogInfo("Excluding satisfying goal node %s to avoid recursive plan expansion", m_pOlcbpPlan->GetNode(satisfyingGoalNode)->ToString().c_str());
                // Add belonging case to exclusion to avoid recursive expansion of plans
                _ASSERTE(GetNodeData(satisfyingGoalNode).BelongingCase != nullptr);
                options.ExcludedGoalHashes.insert(GetNodeData(satisfyingGoalNode).BelongingCase->Goal()->Hash());
            }

            pCurrentGoalNode->AdaptParameters(*g_Game);
            options.GoalTypeId = (GoalType)pCurrentGoalNode->StepTypeId();
            options.pGameState = g_Game;

            options.Parameters = pCurrentGoalNode->Parameters();
            CaseEx* pCandidateCase = m_pCbReasoner->Retriever()->Retrieve(options);

            // We found a matching case and it was not tried for that goal before
            if (pCandidateCase != nullptr)
            {
                // Retriever should always retrieve a non tried case for that specific node
                _ASSERTE(!IsCaseTried(currentNode, pCandidateCase));

                LogInfo("Retrieved case '%s' has not been tried before, and its goal is being sent for expansion",
                    pCandidateCase->Goal()->ToString().c_str());

                MarkCaseAsTried(currentNode, pCandidateCase);
                ExpandGoal(currentNode, pCandidateCase);
            }
            else
            {
                // The current failed goal node is the root goal and the planner exhausted all possible 
                // plans form the case-base for that goal node and nothing succeeded so far
                //
                // WE SURRENDER!!
                //
                if (m_planRootNodeId == currentNode)
                {
                    LogWarning("Planner has exhausted all possible cases, WE SURRENDER!!");
                    m_planRootNodeId = IOlcbpPlan::NullNodeID;
                }
                else
                {
                    LogInfo("Goal=%s exhausted all possible cases, failing it", pCurrentGoalNode->ToString().c_str());
                    pCurrentGoalNode->State(ESTATE_Failed, *g_Game, clock);
                    CloseNode(currentNode);
                    OnGoalNodeFailed(currentNode);
                }
            }
        }
    }
#pragma endregion
#pragma region Node Closed
    else
    {
        _ASSERTE(pCurrentGoalNode->State() == ESTATE_NotPrepared);

        if (pCurrentGoalNode->SuccessConditionsSatisfied(*g_Game))
        {
            pCurrentGoalNode->State(ESTATE_Succeeded, *g_Game, clock);
            OnGoalNodeSucceeded(currentNode);
        }
        else
        {
            // The goal is not done yet, and all of its children are done and
            // finished execution. It does not make sense for the goal to continue
            // this goal should fail, it has no future
            if (GetNodeData(currentNode).WaitOnChildrenCount == 0)
            {
                LogInfo("Goal '%s' is still not done and all of its children are done execution, failing it", 
                    pCurrentGoalNode->ToString().c_str());
                OpenNode(currentNode);
            }
        }
    }
#pragma endregion
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::OnlinePlanExpansionExecution::UpdateActionNode(_In_ IOlcbpPlan::NodeID currentNode, _In_ const WorldClock& clock)
{
    PlanStepEx *pCurrentPlanStep  = m_pOlcbpPlan->GetNode(currentNode);
    _ASSERTE(IsNodeReady(currentNode));

    if (!IsNodeDone(currentNode))
    {
        _ASSERTE(pCurrentPlanStep->State() == ESTATE_NotPrepared ||
            pCurrentPlanStep->State() == ESTATE_Executing ||
            pCurrentPlanStep->State() == ESTATE_END);

        pCurrentPlanStep->Update(*g_Game, clock);

        if (pCurrentPlanStep->State() == ESTATE_Succeeded)
        {
            OnActionNodeSucceeded(currentNode);
        }
        else if (pCurrentPlanStep->State() == ESTATE_Failed)
        {
            OnActionNodeFailed(currentNode);
        }
    }
}