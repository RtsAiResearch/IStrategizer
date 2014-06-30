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

OnlinePlanExpansionExecution::OnlinePlanExpansionExecution(_In_ GoalEx* pInitialGoal, _In_ CaseBasedReasonerEx *pCasedBasedReasoner)
    : EngineComponent("OnlinePlanner"),
    m_planStructureChangedThisFrame(false),
    m_pCbReasoner(pCasedBasedReasoner),
    m_pOlcbpPlan(new OlcbpPlan)
{
    PlanStepEx* pRootNode = (PlanStepEx*)pInitialGoal;
    m_planRootNodeId = m_pOlcbpPlan->AddNode(pRootNode, pRootNode->Id());

    m_nodeData[m_planRootNodeId] = OlcbpPlanNodeData();
    OpenNode(m_planRootNodeId);

    g_MessagePump.RegisterForMessage(MSG_EntityCreate, this);
    g_MessagePump.RegisterForMessage(MSG_EntityDestroy, this);
    g_MessagePump.RegisterForMessage(MSG_EntityRenegade, this);
}
//////////////////////////////////////////////////////////////////////////
OnlinePlanExpansionExecution::OnlinePlanExpansionExecution(_In_ GoalType goalType, _In_ CaseBasedReasonerEx *pCasedBasedReasoner)
    : EngineComponent("OnlinePlanner"),
    m_planStructureChangedThisFrame(false),
    m_pCbReasoner(pCasedBasedReasoner),
    m_pOlcbpPlan(new OlcbpPlan)
{
    m_rootGoalType = goalType;

    g_MessagePump.RegisterForMessage(MSG_EntityCreate, this);
    g_MessagePump.RegisterForMessage(MSG_EntityDestroy, this);
    g_MessagePump.RegisterForMessage(MSG_EntityRenegade, this);
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::ExpandGoal(_In_ IOlcbpPlan::NodeID expansionGoalNodeId, _In_ CaseEx *pExpansionCase)
{
    LogActivity(ExpandGoal);

    IOlcbpPlan *pCasePlan = pExpansionCase->Plan();
    IOlcbpPlan::NodeSerializedSet casePlanRoots = pCasePlan->GetOrphanNodes();
    IOlcbpPlan::NodeSerializedSet casePlanNodes = pCasePlan->GetNodes();
    IOlcbpPlan::NodeSerializedSet newExpansionPlanRoots;

    LogInfo("Expanding node[%d] with plan %s", expansionGoalNodeId, pExpansionCase->Plan()->ToString().c_str());

    // 1. Construct planner plan graph nodes from the expansion case
    SetGoalNodeBelongingCase(expansionGoalNodeId, pExpansionCase);

    // Cross mapping between node IDs in in the planner plan and case plan
    std::map<IOlcbpPlan::NodeID, IOlcbpPlan::NodeID> plannerToCasePlanNodeIdMap;
    std::map<IOlcbpPlan::NodeID, IOlcbpPlan::NodeID> casePlanToPlannerNodeIdMap;

    for(auto caseNodeId : casePlanNodes)
    {
        IOlcbpPlan::NodeValue pOriginalNode = pCasePlan->GetNode(caseNodeId);
        IOlcbpPlan::NodeValue pNode = static_cast<PlanStepEx*>(const_cast<PlanStepEx*>(pOriginalNode)->Clone());

        IOlcbpPlan::NodeID plannerNodeId = m_pOlcbpPlan->AddNode(pNode, pNode->Id());
        m_nodeData[plannerNodeId] = OlcbpPlanNodeData();
        m_nodeData[plannerNodeId].ID = plannerNodeId;

        // Add Data record for the new node
        SetNodeSatisfyingGoal(plannerNodeId, expansionGoalNodeId);

        if (casePlanRoots.count(caseNodeId) > 0)
            newExpansionPlanRoots.insert(plannerNodeId);

        if (IsActionNode(plannerNodeId))
        {
            m_clonedNodesMapping[pOriginalNode] = pNode;
        }
        else if (IsGoalNode(plannerNodeId))
        {
            OpenNode(plannerNodeId);
        }

        // Map node ID in in the planner plan with its counterpart in case plan and vice versa
        plannerToCasePlanNodeIdMap[plannerNodeId] = caseNodeId;
        casePlanToPlannerNodeIdMap[caseNodeId] = plannerNodeId;
    }

    // 2. Link the goal node with the roots of the expansion plan
    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeSerializedSet visitedNodes;

    for(auto rootNodeId : newExpansionPlanRoots)
    {
        // Cross link the goal node with the sub plan roots
        LinkNodes(expansionGoalNodeId, rootNodeId);

        // Enqueue the roots in a queue to expand them
        Q.push(rootNodeId);
        visitedNodes.insert(rootNodeId);
    }

    // 3. Continue construction of the sub plan tree with the rest of the plan graph nodes
    while(!Q.empty())
    {
        IOlcbpPlan::NodeID currPlannerNodeId = Q.front();
        Q.pop();

        IOlcbpPlan::NodeSerializedSet currCaseChildren = pCasePlan->GetAdjacentNodes(plannerToCasePlanNodeIdMap[currPlannerNodeId]);

        for (auto currCaseChildNodeId : currCaseChildren)
        {
            IOlcbpPlan::NodeID currPlannerChildNodeId = casePlanToPlannerNodeIdMap[currCaseChildNodeId];

            // If my child not visited, consider it for expansion
            if(!visitedNodes.count(currPlannerChildNodeId))
            {
                Q.push(currPlannerChildNodeId);
                visitedNodes.insert(currPlannerChildNodeId);
            }

            // Cross link the current node with its child
            LinkNodes(currPlannerNodeId, currPlannerChildNodeId);
        }
    }

    CloseNode(expansionGoalNodeId);

    // 4. Since the plan structure changed, recompute the NotReadyParentsCount
    // for plan graph node
    ComputeFreshSnippetWaitOnParentsCount(expansionGoalNodeId);

    // 5. Record that the expanded goal node is waiting for N number of open nodes
    // If the number of those open nodes = 0, and the goal node is not satisfied, then
    // there is no way for the goal to succeed, and the goal should fail
    GetNodeData(expansionGoalNodeId).SetWaitOnChildrenCount(pExpansionCase->Plan()->Size());

    m_planStructureChangedThisFrame = true;

    LogInfo("Plan after node[%d] expansion %s", expansionGoalNodeId, m_pOlcbpPlan->ToString().c_str());
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::ComputeFreshSnippetWaitOnParentsCount(_In_ IOlcbpPlan::NodeID snippetRootId)
{
    _ASSERTE(m_pOlcbpPlan->Size() > 0);
    _ASSERTE(m_planRootNodeId != IOlcbpPlan::NullNodeID);
    _ASSERTE(!IsNodeOpen(snippetRootId));

    // Root node is excluded from the computation, computation start from the subGraphRoot node adjacent nodes
    LogInfo("Computing graph nodes WaitOnParentsCount property for subGraph with root Node[%d]", snippetRootId);

    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeSerializedSet visitedNodes;
    IOlcbpPlan::NodeSerializedSet snippetRoots;

    GetSnippetOrphanNodes(snippetRootId, snippetRoots);

    for (auto nodeId : snippetRoots)
    {
        // By default a newly expanded nodes should have WaitOnParentsCount = 0
        _ASSERTE(GetNodeData(nodeId).WaitOnParentsCount == 0);
        Q.push(nodeId);
        visitedNodes.insert(nodeId);
    }

    // 2. Do a BFS on the plan graph and for closed node
    // increment the NotReadyParentsCount for child node
    //
    while (!Q.empty())
    {
        IOlcbpPlan::NodeID currNodeId = Q.front();
        Q.pop();

        const IOlcbpPlan::NodeSerializedSet& currChildren = m_pOlcbpPlan->GetAdjacentNodes(currNodeId);

        LogInfo("Computing WaitOnParentsCount for node[%d] children", currNodeId);

        for (auto currChildNodeId : currChildren)
        {
            if (visitedNodes.count(currChildNodeId) == 0)
            {
                // By default a newly expanded nodes should have WaitOnParentsCount = 0
                _ASSERTE(GetNodeData(currChildNodeId).WaitOnParentsCount == 0);
                Q.push(currChildNodeId);
                visitedNodes.insert(currChildNodeId);
            }

            GetNodeData(currChildNodeId).IncWaitOnParentsCount();
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::UpdateBelongingSubplanChildrenWithParentReadiness(_In_ IOlcbpPlan::NodeID nodeId)
{
    LogInfo("Updating node '%s' children with their parent readiness", m_pOlcbpPlan->GetNode(nodeId)->ToString().c_str());

    const IOlcbpPlan::NodeSerializedSet& children =  m_pOlcbpPlan->GetAdjacentNodes(nodeId);

    if (IsActionNode(nodeId))
    {
        for (auto childId : children)
        {
            _ASSERTE(GetNodeData(childId).WaitOnParentsCount > 0);

            LogInfo("Update node '%s' with parent readiness", m_pOlcbpPlan->GetNode(childId)->ToString().c_str());
            GetNodeData(childId).DecWaitOnParentsCount();
        }
    }
    else
    {
        for (auto childId : children)
        {
            if (GetNodeData(childId).SatisfyingGoal != nodeId)
            {
                _ASSERTE(GetNodeData(childId).WaitOnParentsCount > 0);
                //if (GetNodeData(childId).WaitOnParentsCount > 0)
                {
                    LogInfo("Update node '%s' with parent readiness", m_pOlcbpPlan->GetNode(childId)->ToString().c_str());
                    GetNodeData(childId).DecWaitOnParentsCount();
                }
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::OnlinePlanExpansionExecution::MarkCaseAsTried(_In_ IOlcbpPlan::NodeID nodeId, _In_ CaseEx* pCase)
{
    _ASSERTE(GetNodeData(nodeId).TriedCases.count(pCase) == 0);

    LogInfo("Marking case '%s'@%p as tried case for node %d", pCase->Goal()->ToString().c_str(), (void*)pCase, nodeId);

    GetNodeData(nodeId).TriedCases.insert(pCase);
}

//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::UpdateHistory(CaseEx* pCase)
{
    IOlcbpPlan *pCasePlan = pCase->Plan();
    IOlcbpPlan::NodeSerializedSet casePlanNodes = pCasePlan->GetNodes();

    for (auto caseNodeId : casePlanNodes)
    {
        IOlcbpPlan::NodeValue pOriginalNode = pCasePlan->GetNode(caseNodeId);

        if (BELONG(ActionType, pCasePlan->GetNode(caseNodeId)->StepTypeId()))
        {
            Action* pOriginalActionNode = (Action*)pOriginalNode;
            Action* pClonedActionNode = (Action*)m_clonedNodesMapping[pOriginalNode];
            pOriginalActionNode->ExecutionHistory(pClonedActionNode->ExecutionHistory());
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::NotifyMessegeSent(_In_ Message* pMessage)
{
    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeID currentPlanStepID;
    bool msgConsumedByAction = false;

    if (m_pOlcbpPlan->Size() == 0 ||
        m_planRootNodeId == IOlcbpPlan::NullNodeID)
        return;

    Q.push(m_planRootNodeId);

    while(!Q.empty())
    {
        currentPlanStepID = Q.front();
        Q.pop();

        IOlcbpPlan::NodeValue pCurreNode = m_pOlcbpPlan->GetNode(currentPlanStepID);

        if (IsActionNode(currentPlanStepID) && !msgConsumedByAction)
        {
            pCurreNode->HandleMessage(*g_Game, pMessage, msgConsumedByAction);

            if (msgConsumedByAction)
                LogInfo("Message with ID=%d consumed by action node %s", pMessage->MessageTypeID(), pCurreNode->ToString().c_str());
        }

        if (msgConsumedByAction)
        {
            break;
        }

        AddReadyChildrenToUpdateQueue(currentPlanStepID, Q);
    }
}
//////////////////////////////////////////////////////////////////////////
bool OnlinePlanExpansionExecution::DestroyGoalSnippetIfExist(_In_ IOlcbpPlan::NodeID snippetGoalId)
{
    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeSerializedSet visitedNodes;
    IOlcbpPlan::NodeSerializedSet snippetRoots;

    _ASSERTE(IsNodeOpen(snippetGoalId));

    // 1. Collect the snippet orphan nodes to start the BFS from
    GetSnippetOrphanNodes(snippetGoalId, snippetRoots);
    for (auto childNode : snippetRoots)
    {
        if (GetNodeData(childNode).SatisfyingGoal == snippetGoalId)
        {
            Q.push(childNode);
            visitedNodes.insert(childNode);
        }
    }

    // 2. Do a BFS on the snippet to collect its nodes
    while (!Q.empty())
    {
        IOlcbpPlan::NodeID currNodeId = Q.front();
        Q.pop();

        const IOlcbpPlan::NodeSerializedSet& currChildren = m_pOlcbpPlan->GetAdjacentNodes(currNodeId);

        for (auto currChildNodeId : currChildren)
        {
            if (visitedNodes.count(currChildNodeId) == 0)
            {
                Q.push(currChildNodeId);
                visitedNodes.insert(currChildNodeId);
            }
        }
    }

    // 3. Remove visited nodes from the plan
    for (auto visitedNodeId : visitedNodes)
    {
        m_pOlcbpPlan->RemoveNode(visitedNodeId);
        m_nodeData.erase(visitedNodeId);

        LogWarning("MEMORY LEAK detected, should delete plan node[%d]", visitedNodeId);
        // deleting currNode crashes the execution history logic, should fix
        // delete currNode;
    }

    // 4. Since there is no plan destroyed, fast return with false
    if (visitedNodes.empty())
        return false;
    else
    {
        // 4. Since the plan structure changed, raise the flag
        m_planStructureChangedThisFrame = true;
        return true;
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::AddReadyChildrenToUpdateQueue(_In_ IOlcbpPlan::NodeID nodeId, _Inout_ IOlcbpPlan::NodeQueue& updateQ)
{
    const IOlcbpPlan::NodeSerializedSet& children =  m_pOlcbpPlan->GetAdjacentNodes(nodeId);

    for (auto childNodeId : children)
    {
        if (IsNodeReady(childNodeId))
        {
            // If the node is already considered for update, don't add it again to the update queue
            if (find(updateQ._Get_container().begin(),
                updateQ._Get_container().end(),
                childNodeId) == updateQ._Get_container().end())
            {
                // LogInfo("node[%d] is adding node[%d] to update Q", nodeId, childNodeId);
                updateQ.push(childNodeId);
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::OnGoalNodeSucceeded(_In_ IOlcbpPlan::NodeID nodeId)
{
    GoalEx* pGoal = (GoalEx*)m_pOlcbpPlan->GetNode(nodeId);
    _ASSERTE(pGoal->State() == ESTATE_Succeeded);


    if (GetNodeData(nodeId).BelongingCase != nullptr)
    {
        LogInfo("Goal=%s succeeded, revising its case and retaining it", pGoal->ToString().c_str());

        CaseEx* currentCase = GetLastCaseForGoalNode(nodeId);
        m_pCbReasoner->Reviser()->Revise(currentCase, true);
        UpdateHistory(currentCase);
        m_pCbReasoner->Retainer()->Retain(currentCase);
    }
    else
    {
        LogInfo("Goal=%s succeeded without expansion, no case to revise", pGoal->ToString().c_str());
    }

    UpdateBelongingSubplanChildrenWithParentReadiness(nodeId);
    OnNodeDone(nodeId);

    if (m_planRootNodeId == nodeId)
    {
        g_MessagePump.Send(new DataMessage<IOlcbpPlan>(0, MSG_PlanComplete, nullptr));
        m_pOlcbpPlan->Clear();
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::OnGoalNodeFailed(_In_ IOlcbpPlan::NodeID nodeId)
{
    GoalEx* pGoal = (GoalEx*)m_pOlcbpPlan->GetNode(nodeId);
    _ASSERTE(pGoal->State() == ESTATE_Failed);

    if (GetNodeData(nodeId).BelongingCase != nullptr)
    {
        LogInfo("Goal=%s failed, revising and retaining it", pGoal->ToString().c_str());

        CaseEx* currentCase = GetLastCaseForGoalNode(nodeId);
        m_pCbReasoner->Reviser()->Revise(currentCase, false);
        UpdateHistory(currentCase);
        m_pCbReasoner->Retainer()->Retain(currentCase);
    }

    OnNodeDone(nodeId);
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::OnActionNodeSucceeded(_In_ IOlcbpPlan::NodeID nodeId)
{
    UpdateBelongingSubplanChildrenWithParentReadiness(nodeId);
    OnNodeDone(nodeId);
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::OnActionNodeFailed(_In_ IOlcbpPlan::NodeID nodeId)
{
    OnNodeDone(nodeId);
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::OnNodeDone(_In_ IOlcbpPlan::NodeID nodeId)
{
    IOlcbpPlan::NodeID satisfyingGoalNode = GetNodeData(nodeId).SatisfyingGoal;

    if (satisfyingGoalNode != IOlcbpPlan::NullNodeID)
    {
        LogInfo("On '%s' DONE, notifying its satisfying goal '%s'",
            m_pOlcbpPlan->GetNode(nodeId)->ToString().c_str(), m_pOlcbpPlan->GetNode(satisfyingGoalNode)->ToString().c_str());

        _ASSERTE(GetNodeData(satisfyingGoalNode).WaitOnChildrenCount > 0);
        GetNodeData(satisfyingGoalNode).DecWaitOnChildrenCount();
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::GetSnippetOrphanNodes(_In_ IOlcbpPlan::NodeID snippetGoalId, _Out_ IOlcbpPlan::NodeSerializedSet& orphans)
{
    _ASSERTE(orphans.empty());
    for (auto nodeId : m_pOlcbpPlan->GetAdjacentNodes(snippetGoalId))
    {
        if (GetNodeData(nodeId).SatisfyingGoal == snippetGoalId)
            orphans.insert(nodeId);
    }
}
