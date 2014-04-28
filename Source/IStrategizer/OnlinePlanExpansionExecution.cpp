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

using namespace std;
using namespace IStrategizer;

OnlinePlanExpansionExecution::OnlinePlanExpansionExecution(_In_ GoalEx* pInitialGoal, _In_ CaseBasedReasonerEx *pCasedBasedReasoner)
    : EngineComponent("OnlinePlanner"),
    m_planStructureChangedThisFrame(false),
    m_pCbReasoner(pCasedBasedReasoner),
    m_pOlcbpPlan(new OlcbpPlan)
{
    PlanStepEx* pRootNode = (PlanStepEx*)pInitialGoal;
    m_planRootNodeId = m_pOlcbpPlan->AddNode(pRootNode);
    m_nodeData[m_planRootNodeId] = NodeData();

    g_MessagePump.RegisterForMessage(MSG_EntityCreate, this);
    g_MessagePump.RegisterForMessage(MSG_EntityDestroy, this);
    g_MessagePump.RegisterForMessage(MSG_EntityRenegade, this);
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::ExpandGoal(_In_ IOlcbpPlan::NodeID expansionGoalNodeId, _In_ CaseEx *pExpansionCase)
{
    IOlcbpPlan *pCasePlan = pExpansionCase->Plan();
    IOlcbpPlan::NodeSet casePlanRoots = pCasePlan->GetOrphanNodes();
    IOlcbpPlan::NodeSet casePlanNodes = pCasePlan->GetNodes();
    IOlcbpPlan::NodeSet newExpansionPlanRoots;
    IOlcbpPlan::NodeSet preExpansionGoalChildren;

    GetNodeChildrenInBelongingSubplan(expansionGoalNodeId, preExpansionGoalChildren);

    // 1. Construct planner plan graph nodes from the expansion case
    SetGoalNodeBelongingCase(expansionGoalNodeId, pExpansionCase);

    // Cross mapping between node IDs in in the planner plan and case plan
    std::map<IOlcbpPlan::NodeID, IOlcbpPlan::NodeID> plannerToCasePlanNodeIdMap;
    std::map<IOlcbpPlan::NodeID, IOlcbpPlan::NodeID> casePlanToPlannerNodeIdMap;

    for each(auto caseNodeId in casePlanNodes)
    {
        IOlcbpPlan::NodeID plannerNodeId = m_pOlcbpPlan->AddNode(pCasePlan->GetNode(caseNodeId));

        // Map node ID in in the planner plan with its counterpart in case plan and vice versa
        plannerToCasePlanNodeIdMap[plannerNodeId] = caseNodeId;
        casePlanToPlannerNodeIdMap[caseNodeId] = plannerNodeId;

        if (casePlanRoots.count(caseNodeId) > 0)
            newExpansionPlanRoots.insert(plannerNodeId);

        // Add Data record for the new node
        m_nodeData[plannerNodeId] = NodeData();
        SetNodeSatisfyingGoal(plannerNodeId, expansionGoalNodeId);
    }

    // 2. Unlink goal node from its direct children
    UnlinkNodeChildren(expansionGoalNodeId);

    // 3. Link the goal node with the roots of the expansion plan
    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeSet visitedNodes;

    for each(auto rootNodeId in newExpansionPlanRoots)
    {
        // Cross link the goal node with the sub plan roots
        LinkNodes(expansionGoalNodeId, rootNodeId);

        // Enqueue the roots in a queue to expand them
        Q.push(rootNodeId);
        visitedNodes.insert(rootNodeId);
    }

    // 4. Continue construction of the sub plan tree with the rest of the plan graph nodes
    while(!Q.empty())
    {
        IOlcbpPlan::NodeID currPlannerNodeId = Q.front();
        Q.pop();

        IOlcbpPlan::NodeSet currCaseChildren = pCasePlan->GetAdjacentNodes(plannerToCasePlanNodeIdMap[currPlannerNodeId]);

        for each (auto currCaseChildNodeId in currCaseChildren)
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

    // 4. Link the new sub plan leaf nodes with the goal pre-expansion children
    // If the goal had no children before expansion, then there is nothing to link and we are done
    if(!preExpansionGoalChildren.empty())
    {
        IOlcbpPlan::NodeSet casePlanLeaves = pCasePlan->GetLeafNodes();

        for each (auto preExpansionNodeId in preExpansionGoalChildren)
        {
            for each (auto casePlanLeafNodeId in casePlanLeaves)
            {
                IOlcbpPlan::NodeID plannerNodeId = casePlanToPlannerNodeIdMap[casePlanLeafNodeId];

                LinkNodes(plannerNodeId, preExpansionNodeId);
            }
        }
    }

    // 5. Since the plan structure changed, recompute the NotReadyParentsCount
    // for each plan graph node
    ComputeNodesNotReadyParents();

    m_planStructureChangedThisFrame = true;
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::Update(_In_ const WorldClock& clock)
{
    m_pOlcbpPlan->Lock();
    LogInfo("##### PLANNER UPDATE STARTED #####");

    // We have exhausted all possible plans. We have surrendered, nothing to do
    if (m_pOlcbpPlan->Size() > 0)
    {
        IOlcbpPlan::NodeQueue Q;
        IOlcbpPlan::NodeID currentNode;
        IOlcbpPlan::NodeSet planRoots;

        Q.push(m_planRootNodeId);

        while(!Q.empty())
        {
            currentNode = Q.front();
            Q.pop();

            if (IsGoalNode(currentNode))
            {
                UpdateGoalNode(currentNode, clock, Q);
            }
            else if (IsActionNode(currentNode))
            {
                UpdateActionNode(currentNode, clock, Q);
            }
        }
    }

    LogInfo("#### PLANNER UPDATE ENDED ####");
    m_pOlcbpPlan->Unlock();

    if (m_planStructureChangedThisFrame)
    {
         g_MessagePump.Send(new DataMessage<IOlcbpPlan>(0, MSG_PlanStructureChange, nullptr));
         m_planStructureChangedThisFrame = false;
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::UpdateNodeChildrenWithParentReadiness(_In_ IOlcbpPlan::NodeID nodeId)
{
    LogInfo("Updating node '%s' children with their parent readiness", m_pOlcbpPlan->GetNode(nodeId)->ToString().c_str());

    const IOlcbpPlan::NodeSet& children =  m_pOlcbpPlan->GetAdjacentNodes(nodeId);

    for each (auto childId in children)
    {
        _ASSERTE(GetNodeData(childId).NotReadyParentsCount > 0);

        --GetNodeData(childId).NotReadyParentsCount;
    }
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::OnlinePlanExpansionExecution::MarkCaseAsTried(_In_ IOlcbpPlan::NodeID nodeId, _In_ CaseEx* pCase)
{
    _ASSERTE(GetNodeData(nodeId).TriedCases.count(pCase) == 0);

    LogInfo("Marking case '%s'@%x as tried case for node %d", pCase->Goal()->ToString().c_str(), (void*)pCase, nodeId);

    GetNodeData(nodeId).TriedCases.insert(pCase);
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::OnlinePlanExpansionExecution::UpdateGoalNode(_In_ IOlcbpPlan::NodeID currentNode, _In_ const WorldClock& clock, _Inout_ IOlcbpPlan::NodeQueue& updateQ)
{
    PlanStepEx* pCurrentPlanStep = m_pOlcbpPlan->GetNode(currentNode);
    bool hasPreviousPlan = false;

    if (IsNodeOpen(currentNode))
    {
        hasPreviousPlan = DestroyGoalPlanIfExist(currentNode);

        // The goal was previously expanded with a plan, but it somehow failed
        // Thats why it is now open
        // Revise the node belonging case as failed case
        if (hasPreviousPlan)
        {
            LogInfo("Node with plan-step '%s' is open and has children nodes, case is sent for revision and children have been destroyed", pCurrentPlanStep->ToString().c_str());

            m_pCbReasoner->Reviser()->Revise(GetLastCaseForGoalNode(currentNode), false);
            m_pCbReasoner->Retainer()->Retain(GetLastCaseForGoalNode(currentNode));
        }

        CaseEx* caseEx = m_pCbReasoner->Retriever()->Retrieve((GoalEx*)pCurrentPlanStep, g_Game->Self()->State(), GetNodeData(currentNode).TriedCases);
        // Retriever should always retrieve a non tried case for that specific node
        _ASSERTE(!IsCaseTried(currentNode, caseEx));

        // We found a matching case and it was not tried for that goal before
        if (caseEx != nullptr)
        {
            LogInfo("Retrieved case '%s' has not been tried before, and its goal is being sent for expansion",
                caseEx->Goal()->ToString().c_str());

            MarkCaseAsTried(currentNode, caseEx);
            ExpandGoal(currentNode, caseEx);

            CloseNode(currentNode);
            UpdateNodeChildrenWithParentReadiness(currentNode);
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
                LogWarning("Planner has exhausted all possible cases");
                m_planRootNodeId = IOlcbpPlan::NullNodeID;
                m_pOlcbpPlan->Clear();
            }
            else
            {
                LogInfo("GoalNodeID=%d Goal=%s exhausted all cases, failing its case GoalNodeID=%d", currentNode, pCurrentPlanStep->ToString().c_str(), GetNodeData(currentNode).SatisfyingGoal);
                OpenNode(GetNodeData(currentNode).SatisfyingGoal);
            }
        }
    }
    else
    {
        if (pCurrentPlanStep->State() == ESTATE_Failed)
        {
            LogInfo("GoalNodeID=%d Goal=%s failed, opening it", currentNode, pCurrentPlanStep->ToString().c_str());
            OpenNode(currentNode);
        }
        else
        {
            _ASSERTE(
                pCurrentPlanStep->State() == ESTATE_NotPrepared ||
                pCurrentPlanStep->State() == ESTATE_Succeeded ||
                pCurrentPlanStep->State() == ESTATE_END);

            ExecutionStateType oldState = pCurrentPlanStep->State();
            pCurrentPlanStep->Update(*g_Game, clock);
            AddReadyChildrenToUpdateQueue(currentNode, updateQ);

            // This condition will be true just once when the case succeeds
            if (oldState == ESTATE_NotPrepared && pCurrentPlanStep->State() == ESTATE_Succeeded)
            {
                LogInfo("GoalNodeID=%d Goal=%s suceeded, revising and retaining it", currentNode, pCurrentPlanStep->ToString().c_str());
                m_pCbReasoner->Reviser()->Revise(GetLastCaseForGoalNode(currentNode), true);
                m_pCbReasoner->Retainer()->Retain(GetLastCaseForGoalNode(currentNode));
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::OnlinePlanExpansionExecution::UpdateActionNode(_In_ IOlcbpPlan::NodeID currentNode, _In_ const WorldClock& clock, _Inout_ IOlcbpPlan::NodeQueue& updateQ)
{
    PlanStepEx *pCurrentPlanStep  = m_pOlcbpPlan->GetNode(currentNode);
    _ASSERTE(IsNodeReady(currentNode));

    if (IsNodeOpen(currentNode))
    {
        if (pCurrentPlanStep->State() == ESTATE_Failed)
        {
            // We check to make sure that our parent is open before closing
            // Our sub plan goal may be already open because any of our siblings may have failed and opened our sub plan goal
            if (!IsNodeOpen(GetNodeData(currentNode).SatisfyingGoal))
                OpenNode(GetNodeData(currentNode).SatisfyingGoal);
        }
        else if (pCurrentPlanStep->State() == ESTATE_Succeeded)
        {
            CloseNode(currentNode);
            UpdateNodeChildrenWithParentReadiness(currentNode);
        }
        else
        {
            _ASSERTE(pCurrentPlanStep->State() == ESTATE_NotPrepared ||
                pCurrentPlanStep->State() == ESTATE_Executing ||
                pCurrentPlanStep->State() == ESTATE_END);

            pCurrentPlanStep->Update(*g_Game, clock);
        }
    }
    else
    {
        _ASSERTE(pCurrentPlanStep->State() == ESTATE_Succeeded);

        AddReadyChildrenToUpdateQueue(currentNode, updateQ);
    }
}

//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::NotifyMessegeSent(_In_ Message* pMessage)
{
    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeID currentPlanStep;
    bool dummy = false;

    if (m_pOlcbpPlan->Size() == 0 ||
        m_planRootNodeId == IOlcbpPlan::NullNodeID)
        return;

    Q.push(m_planRootNodeId);

    while(!Q.empty())
    {
        currentPlanStep = Q.front();
        Q.pop();

        m_pOlcbpPlan->GetNode(currentPlanStep)->HandleMessage(*g_Game, pMessage, dummy);
        // Obsolete parameter: No one should use the message consuming anymore
        _ASSERTE(dummy == false);

        AddReadyChildrenToUpdateQueue(currentPlanStep, Q);
    }
}
//////////////////////////////////////////////////////////////////////////
bool OnlinePlanExpansionExecution::DestroyGoalPlanIfExist(_In_ IOlcbpPlan::NodeID planGoalNodeId)
{
    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeSet visitedNodes;
    IOlcbpPlan::NodeSet preExpansionChildren;
    IOlcbpPlan::NodeID goalSatisfyingGoal;

    // 1. Do a BFS to collect the pre-expansion children and the goal subplan nodes
    // The visited nodes in the traversal are the goal subplan nodes 
    Q.push(planGoalNodeId);
    visitedNodes.insert(planGoalNodeId);

    goalSatisfyingGoal = GetNodeData(planGoalNodeId).SatisfyingGoal;

    while (!Q.empty())
    {
        IOlcbpPlan::NodeID currNodeId = Q.front();
        Q.pop();

        const IOlcbpPlan::NodeSet& currChildren = m_pOlcbpPlan->GetAdjacentNodes(currNodeId);

        for each (auto currChildNodeId in currChildren)
        {
            if (GetNodeData(currChildNodeId).SatisfyingGoal == goalSatisfyingGoal)
                preExpansionChildren.insert(currChildNodeId);
            else if (visitedNodes.count(currChildNodeId) == 0)
            {
                Q.push(currChildNodeId);
                visitedNodes.insert(currChildNodeId);
            }
        }
    }

    // Remove the goal node from the visited nodes because we don't want to destroy 
    // the goal itself, but the nodes of its expanded subplan
    visitedNodes.erase(planGoalNodeId);

    // 2. Remove visited nodes from the plan
    for each (auto visitedNodeId in visitedNodes)
    {
        m_pOlcbpPlan->RemoveNode(visitedNodeId);
        m_nodeData.erase(visitedNodeId);
    }

    // 3. Link the goal node with the pre expansion children
    for each (auto childNodeId in preExpansionChildren)
    {
        LinkNodes(planGoalNodeId, childNodeId );
    }

    // 4. Since the plan structure changed, recompute the NotReadyParentsCount
    // for each plan graph node
    ComputeNodesNotReadyParents();

    m_planStructureChangedThisFrame = true;

    return !visitedNodes.empty();
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::AddReadyChildrenToUpdateQueue(_In_ IOlcbpPlan::NodeID nodeId, _Inout_ IOlcbpPlan::NodeQueue& updateQ)
{
    const IOlcbpPlan::NodeSet& children =  m_pOlcbpPlan->GetAdjacentNodes(nodeId);

    for each (auto childNodeId in children)
    {
        if (IsNodeReady(childNodeId))
        {
            // If the node is already considered for update, don't add it again to the update queue
            if (find(updateQ._Get_container().begin(),
                updateQ._Get_container().end(),
                childNodeId) == updateQ._Get_container().end())
            {
                updateQ.push(childNodeId);
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
bool OnlinePlanExpansionExecution::IsGoalNode(_In_ IOlcbpPlan::NodeID nodeId) const
{
    IOlcbpPlan::NodeValue pNode = m_pOlcbpPlan->GetNode(nodeId);
    return BELONG(GoalType, pNode->StepTypeId());
}
//////////////////////////////////////////////////////////////////////////
bool OnlinePlanExpansionExecution::IsActionNode(_In_ IOlcbpPlan::NodeID nodeId) const
{
    IOlcbpPlan::NodeValue pNode = m_pOlcbpPlan->GetNode(nodeId);
    return BELONG(ActionType, pNode->StepTypeId());
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::LinkNodes(_In_ IOlcbpPlan::NodeID srcNodeId, _In_ IOlcbpPlan::NodeID dstNodeId)
{
    m_pOlcbpPlan->AddEdge(srcNodeId, dstNodeId);
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::UnlinkNodes(_In_ IOlcbpPlan::NodeID srcNodeId, _In_ IOlcbpPlan::NodeID dstNodeId)
{
    m_pOlcbpPlan->AddEdge(srcNodeId, dstNodeId);
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::GetNodeChildrenInBelongingSubplan(_In_ IOlcbpPlan::NodeID nodeId, _Out_ IOlcbpPlan::NodeSet& children) const
{
    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeSet visitedNodes;
    IOlcbpPlan::NodeID nodeSatisfyingGoal = GetNodeData(nodeId).SatisfyingGoal;

    Q.push(nodeId);
    visitedNodes.insert(nodeId);

    while (!Q.empty())
    {
        IOlcbpPlan::NodeID currNodeId = Q.front();
        Q.pop();

        const IOlcbpPlan::NodeSet& currChildren = m_pOlcbpPlan->GetAdjacentNodes(currNodeId);

        for each (auto currChildNodeId in currChildren)
        {
            if (GetNodeData(currChildNodeId).SatisfyingGoal == nodeSatisfyingGoal)
                children.insert(currChildNodeId);
            else if (visitedNodes.count(currChildNodeId) == 0)
            {
                Q.push(currChildNodeId);
                visitedNodes.insert(currChildNodeId);
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::UnlinkNodeChildren(_In_ IOlcbpPlan::NodeID nodeId)
{
    IOlcbpPlan::NodeSet currChildren = m_pOlcbpPlan->GetAdjacentNodes(nodeId);

    for each (auto currChild in currChildren)
    {
        m_pOlcbpPlan->RemoveEdge(nodeId, currChild);
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::ComputeNodesNotReadyParents()
{
    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeSet visitedNodes;

    _ASSERTE(m_pOlcbpPlan->Size() > 0);
    _ASSERTE(m_planRootNodeId != IOlcbpPlan::NullNodeID);

    // 1. Reset the NotReadyParentsCount field of each node
    //
    IOlcbpPlan::NodeSet planNodes;

    planNodes = m_pOlcbpPlan->GetNodes();
    for each (auto nodeId in planNodes)
    {
        GetNodeData(nodeId).NotReadyParentsCount = 0;
    }

    Q.push(m_planRootNodeId);
    visitedNodes.insert(m_planRootNodeId);

    // 2. Do a BFS on the plan graph and for each closed node
    // increment the NotReadyParentsCount for each child node
    //
    while (!Q.empty())
    {
        IOlcbpPlan::NodeID currNodeId = Q.front();
        Q.pop();

        bool isCurrNodeOpen = GetNodeData(currNodeId).IsOpen;

        const IOlcbpPlan::NodeSet& currChildren = m_pOlcbpPlan->GetAdjacentNodes(currNodeId);

        for each (auto currChildNodeId in currChildren)
        {
            // If current node is open, then update its children to consider the 
            // current open node in the NotReadyParentsCount, by incrementing it by 1
            //
            if (isCurrNodeOpen)
                ++GetNodeData(currChildNodeId).NotReadyParentsCount;

            if (visitedNodes.count(currChildNodeId) == 0)
            {
                Q.push(currChildNodeId);
                visitedNodes.insert(currChildNodeId);
            }
        }
    }
}
