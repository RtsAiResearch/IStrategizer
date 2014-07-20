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
#include "Action.h"
#include "NodeSelectionStrategy.h"
#include <stack>

using namespace std;
using namespace IStrategizer;

//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::Update(_In_ const WorldClock& clock)
{
    m_pOlcbpPlan->Lock();

    // We have exhausted all possible plans. We have surrendered, nothing to do
    if (m_pOlcbpPlan->Size() > 0)
    {
        IOlcbpPlan::NodeSet actionsToUpdate;
        IOlcbpPlan::NodeSet& goalsToUpdate = m_activeGoalSet;
        IOlcbpPlan::NodeSet snippetsToDestroy;

        m_activeGoalSet.clear();
        m_pNodeSelector->Select(goalsToUpdate, actionsToUpdate, snippetsToDestroy);

        for (auto actionNodeId : actionsToUpdate)
        {
            // Only update an action node if it still exist
            // What applies to a goal in the 3rd pass apply here
            _ASSERTE(m_pOlcbpPlan->Contains(actionNodeId));
            UpdateActionNode(actionNodeId, clock);
        }

        for (auto goalNodeId : goalsToUpdate)
        {
            // Only update a goal node if it still exist
            // It is normal that a previous updated goal node during this pass 
            // failed and its snippet was destroyed, and as a result a node that
            // was considered for update does not exist anymore
            if (m_pOlcbpPlan->Contains(goalNodeId))
            {
                // It is illogical to update already succeeding goals, there is
                // a problem in the node selection strategy
                _ASSERTE(m_pOlcbpPlan->GetNode(goalNodeId)->GetState() != ESTATE_Succeeded);

                UpdateGoalNode(goalNodeId, clock);

                // Prune the plan by destroying snippets of succeeding goals to 
                // reduce plan noise and make it easier to read/visualize the plan
                if (m_pOlcbpPlan->GetNode(goalNodeId)->GetState() == ESTATE_Succeeded)
                    snippetsToDestroy.insert(goalNodeId);
            }
        }

        for (auto goalNodeId : snippetsToDestroy)
        {
            if (m_pOlcbpPlan->Contains(goalNodeId))
            {
                (void)DestroyGoalSnippetIfExist(goalNodeId);

                if (m_pOlcbpPlan->GetNode(goalNodeId)->GetState() != ESTATE_Succeeded)
                {
                    OpenNode(goalNodeId);
                    LogInfo("Destroyed %s snippet, it is obsolete",
                        m_pOlcbpPlan->GetNode(goalNodeId)->ToString().c_str());
                }
                else
                {
                    LogInfo("Destroyed %s snippet, its goal succeeded",
                        m_pOlcbpPlan->GetNode(goalNodeId)->ToString().c_str());
                }
            }
        }
    }

    m_pOlcbpPlan->Unlock();

    if (m_planStructureChangedThisFrame)
    {
        g_MessagePump->Send(new Message(clock.ElapsedGameCycles(), MSG_PlanStructureChange));
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
        _ASSERTE(pCurrentGoalNode->GetState() == ESTATE_NotPrepared);
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
        }

        if (pCurrentGoalNode->SuccessConditionsSatisfied(*g_Game))
        {
            LogInfo("Goal %s already satisfied, no need to expand it, closing the node", pCurrentGoalNode->ToString().c_str());
            pCurrentGoalNode->SetState(ESTATE_Succeeded, *g_Game, clock);
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
                // _ASSERTE(!IsCaseTried(currentNode, pCandidateCase));

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
                    pCurrentGoalNode->SetState(ESTATE_Failed, *g_Game, clock);
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
        _ASSERTE(pCurrentGoalNode->GetState() == ESTATE_NotPrepared);

        if (pCurrentGoalNode->SuccessConditionsSatisfied(*g_Game))
        {
            pCurrentGoalNode->SetState(ESTATE_Succeeded, *g_Game, clock);
            OnGoalNodeSucceeded(currentNode);
        }
        else
        {
            // The goal is not done yet, and all of its children are done and
            // finished execution. It does not make sense for the goal to continue
            // this goal should fail, it has no future
            if (GetNodeData(currentNode).WaitOnChildrenCount == 0)
            {
                if (!pCurrentGoalNode->IsSleeping(clock))
                {
                    if (pCurrentGoalNode->SleepsCount() < GoalMaxSleepsCount)
                    {
                        LogInfo("%s is still not done and all of its children are done execution, slept %d time(s) before, will send it to sleep", ToString().c_str(), pCurrentGoalNode->SleepsCount());
                        pCurrentGoalNode->Sleep(clock, GoalSleepTime);
                    }
                    else
                    {
                        LogInfo("%s is still not done and all of its children are done execution, already tried to sleep it %d time(s) before, failing it", pCurrentGoalNode->ToString().c_str(), GoalMaxSleepsCount);
                        OpenNode(currentNode);
                    }
                }
            }
        }
    }
#pragma endregion
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::OnlinePlanExpansionExecution::UpdateActionNode(_In_ IOlcbpPlan::NodeID currentNode, _In_ const WorldClock& clock)
{
    _ASSERTE(IsNodeReady(currentNode));
    _ASSERTE(IsActionNode(currentNode));

    Action *pAction = (Action*)m_pOlcbpPlan->GetNode(currentNode);

    if (!IsNodeDone(currentNode))
    {
        _ASSERTE(pAction->GetState() == ESTATE_NotPrepared ||
            pAction->GetState() == ESTATE_Executing ||
            pAction->GetState() == ESTATE_END);

        pAction->Update(*g_Game, clock);
        AddExecutingAction(currentNode);

        if (pAction->GetState() == ESTATE_Succeeded)
        {
            OnActionNodeSucceeded(currentNode);
        }
        else if (pAction->GetState() == ESTATE_Failed)
        {
            if (pAction->SleepsCount() < ActionMaxSleepsCount)
            {
                LogInfo("%s failed execution, but will reset it and send it to sleep", pAction->ToString().c_str(), pAction->SleepsCount());

                pAction->Abort(*g_Game);
                pAction->Reset(*g_Game, clock);
                pAction->Sleep(clock, ActionSleepTime);
            }
            else
            {

                LogInfo("%s failed execution, already tried to sleep it %d time(s) before, failing it", pAction->ToString().c_str(), ActionMaxSleepsCount);
                OnActionNodeFailed(currentNode);
            }
        }
    }
}

