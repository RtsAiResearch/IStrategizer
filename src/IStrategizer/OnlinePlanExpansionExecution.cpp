#include "OnlinePlanExpansionExecution.h"
#include "CaseBasedReasonerEx.h"
#include "CaseEx.h"
#include "RtsGame.h"
#include "Action.h"
#include "MessagePump.h"
#include "Message.h"
#include <crtdbg.h>
#include <stack>
#include "AbstractReviser.h"
#include "AbstractRetriever.h"
#include "GamePlayer.h"
#include "Toolbox.h"
#include "Logger.h"
#include "RetainerEx.h"
#include "DataMessage.h"
#include "GoalFactory.h"
#include "PlayerResources.h"
#include "EngineDefs.h"
#include "NodeSelectionStrategy.h"

using namespace std;
using namespace IStrategizer;
using namespace Serialization;

OnlinePlanExpansionExecution::OnlinePlanExpansionExecution(_In_ CaseBasedReasonerEx *pCasedBasedReasoner) :
    m_planStructureChangedThisFrame(false),
    m_pCbReasoner(pCasedBasedReasoner),
    m_pOlcbpPlan(new OlcbpPlan),
    m_pPlanGoalPrototype(nullptr),
    m_planContext(m_nodeData, m_activeGoalSet),
    m_pNodeSelector(new GenCbNodeSelector(this)),
    m_inMaintenanceMode(false)
{
    g_MessagePump->RegisterForMessage(MSG_EntityCreate, this);
    g_MessagePump->RegisterForMessage(MSG_EntityDestroy, this);
    g_MessagePump->RegisterForMessage(MSG_EntityRenegade, this);
}
//////////////////////////////////////////////////////////////////////////
OnlinePlanExpansionExecution::~OnlinePlanExpansionExecution()
{
    ClearPlan();
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::ClearPlan()
{
    auto nodes = m_pOlcbpPlan->GetNodes();
    for (auto nodeId : nodes)
    {
        auto pNode = m_pOlcbpPlan->GetNode(nodeId);
        delete pNode;
    }
    m_pOlcbpPlan->Clear();
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::StartNewPlan(_In_ GoalEx* pPlanGoal)
{
    ClearPlan();

    LogInfo("Starting a new plan %s", pPlanGoal->ToString().c_str());

    // Release previous plan goal
    m_pPlanGoalPrototype.reset();
    m_pPlanGoalPrototype = shared_ptr<GoalEx>((GoalEx*)pPlanGoal->Clone());
    m_planRootNodeId = m_pOlcbpPlan->AddNode(pPlanGoal, pPlanGoal->Id());

    m_nodeData[m_planRootNodeId] = OlcbpPlanNodeData();
    OpenNode(m_planRootNodeId);

    m_planStructureChangedThisFrame = true;
    m_inMaintenanceMode = false;
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

    int nodeAddRemoveDelta = 0; //AdaptSnippet(expansionGoalNodeId);
    // Adaptation should leave at least 1 node in the snippet
    _ASSERTE((int)pCasePlan->Size() + nodeAddRemoveDelta > 0);
    size_t adaptedSnippetSize = (size_t)((int)pCasePlan->Size() + nodeAddRemoveDelta);

    CloseNode(expansionGoalNodeId);

    // 4. Since the plan structure changed, recompute the NotReadyParentsCount
    // for plan graph node
    ComputeFreshSnippetWaitOnParentsCount(expansionGoalNodeId);

    // 5. Record that the expanded goal node is waiting for N number of open nodes
    // If the number of those open nodes = 0, and the goal node is not satisfied, then
    // there is no way for the goal to succeed, and the goal should fail
    GetNodeData(expansionGoalNodeId).SetWaitOnChildrenCount(adaptedSnippetSize);

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
    //_ASSERTE(GetNodeData(nodeId).TriedCases.count(pCase) == 0);

    LogInfo("Marking case '%s'@%p as tried case for node %d", pCase->Goal()->ToString().c_str(), (void*)pCase, nodeId);

    GetNodeData(nodeId).TriedCases.insert(pCase);
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::OnlinePlanExpansionExecution::MarkActionAsInactive(IOlcbpPlan::NodeID nodeId)
{
    bool wasActive = false;

    for (auto& r : m_activeActions)
    {
        wasActive |= (r.second.erase(nodeId) > 0);
    }

    if (wasActive)
    {
        LogInfo("Action %s is not active anymore", m_pOlcbpPlan->GetNode(nodeId)->ToString().c_str());
    }
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::OnlinePlanExpansionExecution::MarkActionAsActive(IOlcbpPlan::NodeID nodeId)
{
    IOlcbpPlan::NodeSet ancestors;
    GetAncestorSatisfyingGoals(nodeId, ancestors);
    for (IOlcbpPlan::NodeID ancestor : ancestors)
        m_activeActions[ancestor].insert(nodeId);

    LogInfo("Action %s is active", m_pOlcbpPlan->GetNode(nodeId)->ToString().c_str());
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
            pOriginalActionNode->SetExecutionHistory(pClonedActionNode->GetExecutionHistory());
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

        if (IsActionNode(currentPlanStepID) &&
            pCurreNode->GetState() == ESTATE_Executing)
        {
            pCurreNode->HandleMessage(pMessage, msgConsumedByAction);

            if (msgConsumedByAction)
            {
                LogInfo("Message with ID=%d consumed by action node %s", pMessage->TypeId(), pCurreNode->ToString().c_str());
                break;
            }
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
        auto pCurrNode = m_pOlcbpPlan->GetNode(visitedNodeId);

        if (IsActionNode(visitedNodeId))
        {
            ((Action*)pCurrNode)->Abort();
            MarkActionAsInactive(visitedNodeId);
        }

        // FIXME: deleting currNode crashes the execution history logic, should fix
        m_pOlcbpPlan->RemoveNode(visitedNodeId);
        m_nodeData.erase(visitedNodeId);
        delete pCurrNode;
    }

    // 4. Since there is no plan destroyed, fast return with false
    if (visitedNodes.empty())
        return false;
    else
    {
        GetNodeData(snippetGoalId).SetWaitOnChildrenCount(0);
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
    _ASSERTE(pGoal->GetState() == ESTATE_Succeeded);

    if (GetNodeData(nodeId).BelongingCase != nullptr)
    {
        LogInfo("Goal=%s succeeded, revising its case and retaining it", pGoal->ToString().c_str());

        CaseEx* currentCase = GetLastCaseForGoalNode(nodeId);
        m_pCbReasoner->Reviser()->Revise(currentCase, true);
        //UpdateHistory(currentCase);
    }
    else
    {
        LogInfo("Goal=%s succeeded without expansion, no case to revise", pGoal->ToString().c_str());
    }

    UpdateBelongingSubplanChildrenWithParentReadiness(nodeId);
    OnNodeDone(nodeId);

    if (m_planRootNodeId == nodeId)
    {
        g_MessagePump->Send(new Message(0, MSG_PlanGoalSuccess));
        LogInfo("Plan goal succeeded, entering maintenance mode");
        m_inMaintenanceMode = true;
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::OnGoalNodeFailed(_In_ IOlcbpPlan::NodeID nodeId)
{
    GoalEx* pGoal = (GoalEx*)m_pOlcbpPlan->GetNode(nodeId);
    _ASSERTE(pGoal->GetState() == ESTATE_Failed);

    if (GetNodeData(nodeId).BelongingCase != nullptr)
    {
        LogInfo("Goal=%s failed, revising and retaining it", pGoal->ToString().c_str());

        CaseEx* currentCase = GetLastCaseForGoalNode(nodeId);
        m_pCbReasoner->Reviser()->Revise(currentCase, false);
        //UpdateHistory(currentCase);
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

    if (IsActionNode(nodeId))
        MarkActionAsInactive(nodeId);

    if (satisfyingGoalNode != IOlcbpPlan::NullNodeID)
    {
        LogInfo("On '%s' DONE, notifying its satisfying goal '%s'",
            m_pOlcbpPlan->GetNode(nodeId)->ToString().c_str(), m_pOlcbpPlan->GetNode(satisfyingGoalNode)->ToString().c_str());

        _ASSERTE(GetNodeData(satisfyingGoalNode).WaitOnChildrenCount > 0);
        GetNodeData(satisfyingGoalNode).DecWaitOnChildrenCount();
    }

    if (IsGoalNode(nodeId) && m_backupNodes.count(nodeId) != 0)
    {
        LogInfo("A backup node is done %s", m_pOlcbpPlan->GetNode(nodeId)->ToString().c_str());
        if (DestroyGoalSnippetIfExist(nodeId))
            m_pOlcbpPlan->RemoveNode(nodeId);
        m_coverGoals.erase(m_backupNodes[nodeId]);
        m_backupNodes.erase(nodeId);
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
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::CoverFailedGoals()
{
    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeID currNodeId;
    IOlcbpPlan::NodeSerializedSet planRoots;
    IOlcbpPlan::NodeSet visitedNodes;
    IOlcbpPlan::NodeSet toCover;

    _ASSERTE(m_planRootNodeId != IOlcbpPlan::NullNodeID);

    Q.push(m_planRootNodeId);
    visitedNodes.insert(m_planRootNodeId);

    while (!Q.empty())
    {
        currNodeId = Q.front();
        Q.pop();

        if (!m_pOlcbpPlan->Contains(currNodeId))
        {
            LogWarning("A non existing node was there in the update queue, skipping it");
            continue;
        }

        for (auto childNodeId : m_pOlcbpPlan->GetAdjacentNodes(currNodeId))
        {
            if (visitedNodes.count(childNodeId) == 0)
            {
                if (IsGoalNode(childNodeId) && IsNodeDone(childNodeId))
                {
                    auto goalNode = m_pOlcbpPlan->GetNode(childNodeId);
                    if (m_coverGoals.count(childNodeId) == 0 && 
                        !goalNode->SuccessConditionsSatisfied(*g_Game) &&
                        m_backupNodes.count(childNodeId) == 0)
                    {
                        m_coverGoals.insert(childNodeId);
                        toCover.insert(childNodeId);
                    }
                }

                visitedNodes.insert(childNodeId);
                Q.push(childNodeId);
            }
        }
    }

    for (auto nodeId : toCover)
    {
        auto goalNode = m_pOlcbpPlan->GetNode(nodeId);
        GoalEx* backupNode = (GoalEx*)goalNode->Clone();
        m_backupNodes.insert(MakePair(backupNode->Id(), nodeId));
        backupNode->SetState(ESTATE_NotPrepared);
        m_pOlcbpPlan->AddNode(backupNode, backupNode->Id());
        m_nodeData[backupNode->Id()] = OlcbpPlanNodeData();
        m_nodeData[backupNode->Id()].ID = backupNode->Id();
        SetNodeSatisfyingGoal(backupNode->Id(), m_planRootNodeId);
        LinkNodes(m_planRootNodeId, backupNode->Id());
        auto& rootNodeData = GetNodeData(m_planRootNodeId);
        rootNodeData.SetWaitOnChildrenCount(rootNodeData.WaitOnChildrenCount + 1);
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::GetReachableReadyNodes(_Out_ IOlcbpPlan::NodeQueue& actionQ, _Out_ IOlcbpPlan::NodeQueue& goalQ) const
{
    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeID currNodeId;
    IOlcbpPlan::NodeSerializedSet planRoots;
    IOlcbpPlan::NodeSet visitedNodes;

    _ASSERTE(m_planRootNodeId != IOlcbpPlan::NullNodeID);

    Q.push(m_planRootNodeId);
    visitedNodes.insert(m_planRootNodeId);
    goalQ.push(m_planRootNodeId);

    // Do a BFS on the plan an collect only ready nodes (i.e nodes with WaitOnParentsCount = 0)
    // Add ready action nodes to the action Q
    // Add ready goal nodes to the goal Q
    while(!Q.empty())
    {
        currNodeId = Q.front();
        Q.pop();

        if (!m_pOlcbpPlan->Contains(currNodeId))
        {
            LogWarning("A non existing node was there in the update queue, skipping it");
            continue;
        }

        for (auto childNodeId : m_pOlcbpPlan->GetAdjacentNodes(currNodeId))
        {
            if (visitedNodes.count(childNodeId) == 0)
            {
                if (IsNodeReady(childNodeId))
                {
                    if (IsGoalNode(childNodeId))
                    {
                        goalQ.push(childNodeId);
                    }
                    else if (IsActionNode(childNodeId))
                    {
                        actionQ.push(childNodeId);
                    }

                    Q.push(childNodeId);
                }

                visitedNodes.insert(childNodeId);
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
int OnlinePlanExpansionExecution::AdaptSnippet(_In_ IOlcbpPlan::NodeID snippetRootGoalId)
{
    int nodeAddRemoveDelta = 0;
    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeSerializedSet planRoots;
    IOlcbpPlan::NodeSet visitedNodes;
    IOlcbpPlan::NodeID parentNodeId = snippetRootGoalId;

    Q.push(parentNodeId);
    visitedNodes.insert(parentNodeId);

    // Do a BFS on the plan an collect only ready nodes (i.e nodes with WaitOnParentsCount = 0)
    // Add ready action nodes to the action Q
    // Add ready goal nodes to the goal Q
    while(!Q.empty())
    {
        parentNodeId = Q.front();
        Q.pop();

        IOlcbpPlan::NodeSerializedSet originalSnippetNodes = m_pOlcbpPlan->GetAdjacentNodes(parentNodeId);
        for (auto childNodeId : originalSnippetNodes)
        {
            if (visitedNodes.count(childNodeId) != 0)
                continue;

            if (IsActionNode(childNodeId))
            {
                ((Action*)m_pOlcbpPlan->GetNode(childNodeId))->InitializeConditions();
                for (auto pExpression : ((Action*)m_pOlcbpPlan->GetNode(childNodeId))->PreCondition()->Expressions())
                {
                    _ASSERTE(pExpression->ExpressionType() == EXPRESSION_Leaf);
                    if (((ConditionEx*)pExpression)->ContainsParameter(PARAM_ResourceId) &&
                        ((ConditionEx*)pExpression)->Parameter(PARAM_ResourceId) == RESOURCE_Supply)
                    {
                        int requiredSupplyAmmount = ((ConditionEx*)pExpression)->Parameter(PARAM_Amount);

                        if (g_Game->Self()->Resources()->AvailableSupply() >= requiredSupplyAmmount)
                            continue;

                        UnlinkNodes(parentNodeId, childNodeId);

                        PlanStepParameters params;
                        params[PARAM_EntityClassId] = g_Game->Self()->Race()->GetResourceSource(RESOURCE_Supply);
                        params[PARAM_Amount] = 1;

                        auto pBuildInfraGoal = g_GoalFactory.GetGoal(GOALEX_BuildInfrastructure, params, true);
                        IOlcbpPlan::NodeID buildInfroNodeId = m_pOlcbpPlan->AddNode(pBuildInfraGoal, pBuildInfraGoal->Id());
                        m_nodeData[pBuildInfraGoal->Id()] = OlcbpPlanNodeData();
                        m_nodeData[pBuildInfraGoal->Id()].ID = pBuildInfraGoal->Id();

                        // Add Data record for the new node
                        SetNodeSatisfyingGoal(pBuildInfraGoal->Id(), snippetRootGoalId);

                        LinkNodes(parentNodeId, buildInfroNodeId);
                        LinkNodes(buildInfroNodeId, childNodeId);
                        ++nodeAddRemoveDelta;
                    }
                }
            }

            Q.push(childNodeId);
            visitedNodes.insert(childNodeId);
        }
    }

    return nodeAddRemoveDelta;
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::GetAncestorSatisfyingGoals(_In_ IOlcbpPlan::NodeID nodeId, _Out_ IOlcbpPlan::NodeSet& ancestors) const
{
    nodeId = GetNodeData(nodeId).SatisfyingGoal;

    while (nodeId != IOlcbpPlan::NullNodeID)
    {
        ancestors.insert(nodeId);
        nodeId = GetNodeData(nodeId).SatisfyingGoal;
    }
}
//////////////////////////////////////////////////////////////////////////
bool OnlinePlanExpansionExecution::IsGoalExpanded(_In_ IOlcbpPlan::NodeID snippetGoalId)
{
    for (auto childNodeId : m_pOlcbpPlan->GetAdjacentNodes(snippetGoalId))
    {
        // Goal is not expanded with a snippet as long as the goal is not
        // the satisfying goal of any direct child
        if (GetNodeData(childNodeId).SatisfyingGoal == snippetGoalId)
            return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
bool OnlinePlanExpansionExecution::IsPlanDone()
{
    for (auto& a : m_activeActions)
    {
        if (!a.second.empty())
            return false;
    }

    return true;
}