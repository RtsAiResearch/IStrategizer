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
    OpenNode(m_planRootNodeId);

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

    for(auto caseNodeId : casePlanNodes)
    {
        IOlcbpPlan::NodeValue pOriginalNode = pCasePlan->GetNode(caseNodeId);
        IOlcbpPlan::NodeValue pNode = static_cast<PlanStepEx*>(const_cast<PlanStepEx*>(pOriginalNode)->Clone());
        IOlcbpPlan::NodeID plannerNodeId = m_pOlcbpPlan->AddNode(pNode);
        m_nodeData[plannerNodeId] = NodeData();
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

    // 2. Unlink goal node from its direct children
    // UnlinkNodeChildren(expansionGoalNodeId);

    // 3. Link the goal node with the roots of the expansion plan
    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeSet visitedNodes;

    for(auto rootNodeId : newExpansionPlanRoots)
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

    // 4. Link the new sub plan leaf nodes with the goal pre-expansion children
    // If the goal had no children before expansion, then there is nothing to link and we are done
    //if(!preExpansionGoalChildren.empty())
    //{
    //    IOlcbpPlan::NodeSet casePlanLeaves = pCasePlan->GetLeafNodes();

    //    for (auto preExpansionNodeId : preExpansionGoalChildren)
    //    {
    //        for (auto casePlanLeafNodeId : casePlanLeaves)
    //        {
    //            IOlcbpPlan::NodeID plannerNodeId = casePlanToPlannerNodeIdMap[casePlanLeafNodeId];

    //            LinkNodes(plannerNodeId, preExpansionNodeId);
    //        }
    //    }
    //}

    // 5. Since the plan structure changed, recompute the NotReadyParentsCount
    // for plan graph node
    ComputeNodesWaitOnParentsCount();

    // 6. Record that the expanded goal node is waiting for N number of open nodes
    // If the number of those open nodes = 0, and the goal node is not satisfied, then
    // there is no way for the goal to succeed, and the goal should fail
    GetNodeData(expansionGoalNodeId).SetWaitOnChildrenCount(pExpansionCase->Plan()->Size());

    m_planStructureChangedThisFrame = true;
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::Update(_In_ const WorldClock& clock)
{
    m_pOlcbpPlan->Lock();

    LogInfo("### START PLAN UPDATE ###");
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

            if (m_pOlcbpPlan->GetNode(currentNode) == nullptr)
                LogWarning("A non existing node was there in the update queue, skipping it");

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

    LogInfo("### END PLAN UPDATE ###");

    m_pOlcbpPlan->Unlock();

    if (m_planStructureChangedThisFrame)
    {
        g_MessagePump.Send(new DataMessage<IOlcbpPlan>(0, MSG_PlanStructureChange, nullptr));
        m_planStructureChangedThisFrame = false;
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::UpdateBelongingSubplanChildrenWithParentReadiness(_In_ IOlcbpPlan::NodeID nodeId)
{
    LogInfo("Updating node '%s' children with their parent readiness", m_pOlcbpPlan->GetNode(nodeId)->ToString().c_str());

    const IOlcbpPlan::NodeSet& children =  m_pOlcbpPlan->GetAdjacentNodes(nodeId);

    if (IsActionNode(nodeId))
    {
        for (auto childId : children)
        {
            _ASSERTE(GetNodeData(childId).WaitOnParentsCount > 0);

            LogInfo("Update node[%d] '%s' with parent readiness", childId, m_pOlcbpPlan->GetNode(childId)->ToString().c_str());
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

                LogInfo("Update node[%d] '%s' with parent readiness", childId, m_pOlcbpPlan->GetNode(childId)->ToString().c_str());
                GetNodeData(childId).DecWaitOnParentsCount();
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::UpdateGoalPlanChildrenWithParentReadiness(_In_ IOlcbpPlan::NodeID nodeId)
{
    _ASSERTE(IsGoalNode(nodeId));
    LogInfo("Updating node '%s' children with their parent readiness", m_pOlcbpPlan->GetNode(nodeId)->ToString().c_str());

    const IOlcbpPlan::NodeSet& children =  m_pOlcbpPlan->GetAdjacentNodes(nodeId);

    for (auto childId : children)
    {
        _ASSERTE(GetNodeData(childId).WaitOnParentsCount > 0);

        if (GetNodeData(childId).SatisfyingGoal == nodeId)
        {
            LogInfo("Update node[%d] '%s' with parent readiness", childId, m_pOlcbpPlan->GetNode(childId)->ToString().c_str());
            GetNodeData(childId).DecWaitOnParentsCount();
        }
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

    if (IsNodeDone(currentNode) &&
        pCurrentPlanStep->State() == ESTATE_Succeeded)
    {
        AddReadyChildrenToUpdateQueue(currentNode, updateQ);
    }
    else if (IsNodeOpen(currentNode))
    {
        hasPreviousPlan = DestroyGoalPlanIfExist(currentNode);

        // The goal was previously expanded with a plan, but it somehow failed
        // Thats why it is now open
        // Revise the node belonging case as failed case
        if (hasPreviousPlan)
        {
            LogInfo("Node with plan-step '%s' is open and has children nodes, case is sent for revision and children have been destroyed", pCurrentPlanStep->ToString().c_str());

            CaseEx* currentCase = GetLastCaseForGoalNode(currentNode);
            m_pCbReasoner->Reviser()->Revise(currentCase, false);
            UpdateHistory(currentCase);
            m_pCbReasoner->Retainer()->Retain(currentCase);
            m_pCbReasoner->Retainer()->Flush();
        }
        // This is the node first time expansion
        else
        {
            GoalEx* currentGoalNode = (GoalEx*)pCurrentPlanStep;
            if (clock.ElapsedGameCycles() > 1 && currentGoalNode->SuccessConditionsSatisfied(*g_Game))
            {
                LogInfo("Goal already satisfied, no need to expand it");
            }
            else
            {
                CaseSet exclusions = GetNodeData(currentNode).TriedCases;

                IOlcbpPlan::NodeID satisfyingGoalNode = GetNodeData(currentNode).SatisfyingGoal;

                if (satisfyingGoalNode != IOlcbpPlan::NullNodeID)
                {
                    // Add belonging case to exclusion to avoid recursive expansion of plans
                    _ASSERTE(GetNodeData(satisfyingGoalNode).BelongingCase != nullptr);
                    exclusions.insert(GetNodeData(satisfyingGoalNode).BelongingCase);
                }

                currentGoalNode->AdaptParameters(*g_Game);
                CaseEx* caseEx = m_pCbReasoner->Retriever()->Retrieve(currentGoalNode, g_Game->Self()->State(), exclusions);
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
                    UpdateGoalPlanChildrenWithParentReadiness(currentNode);
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
                        LogInfo("Goal=%s with NodeID=%d exhausted all possible cases, failing it", pCurrentPlanStep->ToString().c_str(), currentNode);
                        pCurrentPlanStep->State(ESTATE_Failed, *g_Game, clock);
                        MarkNodeAsDone(currentNode);
                        // OpenNode(GetNodeData(currentNode).SatisfyingGoal);
                    }
                }
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
        else if (pCurrentPlanStep->State() == ESTATE_Succeeded)
        {
            LogInfo("GoalNodeID=%d Goal=%s suceeded, revising and retaining it", currentNode, pCurrentPlanStep->ToString().c_str());
            CaseEx* currentCase = GetLastCaseForGoalNode(currentNode);
            m_pCbReasoner->Reviser()->Revise(currentCase, true);
            UpdateHistory(currentCase);
            m_pCbReasoner->Retainer()->Retain(currentCase);
            m_pCbReasoner->Retainer()->Flush();

            MarkNodeAsDone(currentNode);
            UpdateBelongingSubplanChildrenWithParentReadiness(currentNode);
            AddReadyChildrenToUpdateQueue(currentNode, updateQ);
        }
        else
        {
            _ASSERTE(
                pCurrentPlanStep->State() == ESTATE_NotPrepared ||
                pCurrentPlanStep->State() == ESTATE_END);

            pCurrentPlanStep->Update(*g_Game, clock);

            // The goal is not done yet, and all of its children are done and
            // finished execution. It does not make sense for the goal to continue
            // this goal should fail
            if (pCurrentPlanStep->State() == ESTATE_NotPrepared &&
                GetNodeData(currentNode).WaitOnChildrenCount == 0)
            {
                LogInfo("Goal '%s' NodeID=%d is still not done and all of its children are done execution, failing it", 
                    pCurrentPlanStep->ToString().c_str(), currentNode);
                pCurrentPlanStep->State(ESTATE_Failed, *g_Game, clock);
            }
            else
            {
                AddReadyChildrenToUpdateQueue(currentNode, updateQ);
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::OnlinePlanExpansionExecution::UpdateActionNode(_In_ IOlcbpPlan::NodeID currentNode, _In_ const WorldClock& clock, _Inout_ IOlcbpPlan::NodeQueue& updateQ)
{
    PlanStepEx *pCurrentPlanStep  = m_pOlcbpPlan->GetNode(currentNode);
    _ASSERTE(IsNodeReady(currentNode));

    if (IsNodeDone(currentNode))
    {
        if (pCurrentPlanStep->State() == ESTATE_Succeeded)
            AddReadyChildrenToUpdateQueue(currentNode, updateQ);
    }
    else if (pCurrentPlanStep->State() == ESTATE_Failed)
    {
        MarkNodeAsDone(currentNode);

        //    // We check to make sure that our parent is open before closing
        //    // Our sub plan goal may be already open because any of our siblings may have failed and opened our sub plan goal
        //    if (!IsNodeOpen(GetNodeData(currentNode).SatisfyingGoal))
        //        OpenNode(GetNodeData(currentNode).SatisfyingGoal);
    }
    else if (pCurrentPlanStep->State() == ESTATE_Succeeded)
    {
        MarkNodeAsDone(currentNode);
        UpdateBelongingSubplanChildrenWithParentReadiness(currentNode);
    }
    else if (pCurrentPlanStep->State() != ESTATE_Failed)
    {
        _ASSERTE(pCurrentPlanStep->State() == ESTATE_NotPrepared ||
            pCurrentPlanStep->State() == ESTATE_Executing ||
            pCurrentPlanStep->State() == ESTATE_END);

        pCurrentPlanStep->Update(*g_Game, clock);
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::UpdateHistory(CaseEx* pCase)
{
    IOlcbpPlan *pCasePlan = pCase->Plan();
    IOlcbpPlan::NodeSet casePlanNodes = pCasePlan->GetNodes();

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
    bool msgConsumedByGoal = false;

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
                LogInfo("Message with ID=%d consumed by action node with ID=%d, planstep=%s", pMessage->MessageTypeID(), currentPlanStepID, pCurreNode->ToString().c_str());
        }
        else if (IsGoalNode(currentPlanStepID) && !msgConsumedByGoal)
        {
            pCurreNode->HandleMessage(*g_Game, pMessage, msgConsumedByGoal);

            if (msgConsumedByGoal)
                LogInfo("Message with ID=%d consumed by goal node with ID=%d, planstep=%s", pMessage->MessageTypeID(), currentPlanStepID, pCurreNode->ToString().c_str());
        }

        if (msgConsumedByAction && msgConsumedByGoal)
        {
            break;
        }

        AddReadyChildrenToUpdateQueue(currentPlanStepID, Q);
    }
}
//////////////////////////////////////////////////////////////////////////
bool OnlinePlanExpansionExecution::DestroyGoalPlanIfExist(_In_ IOlcbpPlan::NodeID planGoalNodeId)
{
    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeSet visitedNodes;
    IOlcbpPlan::NodeSet preExpansionChildren;
    //IOlcbpPlan::NodeID goalSatisfyingGoal;

    // 1. Do a BFS to collect the pre-expansion children and the goal subplan nodes
    // The visited nodes in the traversal are the goal subplan nodes
    for (auto childNode : m_pOlcbpPlan->GetAdjacentNodes(planGoalNodeId))
    {
        if (GetNodeData(childNode).SatisfyingGoal == planGoalNodeId)
        {
            Q.push(childNode);
            visitedNodes.insert(childNode);
            preExpansionChildren.insert(childNode);
        }
    }

    while (!Q.empty())
    {
        IOlcbpPlan::NodeID currNodeId = Q.front();
        Q.pop();

        const IOlcbpPlan::NodeSet& currChildren = m_pOlcbpPlan->GetAdjacentNodes(currNodeId);

        for (auto currChildNodeId : currChildren)
        {
            if (visitedNodes.count(currChildNodeId) == 0)
            {
                preExpansionChildren.insert(currChildNodeId);

                Q.push(currChildNodeId);
                visitedNodes.insert(currChildNodeId);
            }
        }
    }

    // Remove the goal node from the visited nodes because we don't want to destroy 
    // the goal itself, but the nodes of its expanded subplan
    // visitedNodes.erase(planGoalNodeId);

    // 2. Remove visited nodes from the plan
    for (auto visitedNodeId : visitedNodes)
    {
        //auto currNode = m_pOlcbpPlan->GetNode(visitedNodeId);
        m_pOlcbpPlan->RemoveNode(visitedNodeId);
        m_nodeData.erase(visitedNodeId);
        
        LogWarning("MEMORY LEAK detected, should delete plan node[%d]", visitedNodeId);
        // deleting currNode crashes the execution history logic, should fix
        // delete currNode;
    }

    // 3. Link the goal node with the pre expansion children
    // for (auto childNodeId : preExpansionChildren)
    //  {
    //    LinkNodes(planGoalNodeId, childNodeId );
    //}

    // 4. Since the plan structure changed, recompute the NotReadyParentsCount
    // for plan graph node
    ComputeNodesWaitOnParentsCount();

    m_planStructureChangedThisFrame = true;

    return !visitedNodes.empty();
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::AddReadyChildrenToUpdateQueue(_In_ IOlcbpPlan::NodeID nodeId, _Inout_ IOlcbpPlan::NodeQueue& updateQ)
{
    const IOlcbpPlan::NodeSet& children =  m_pOlcbpPlan->GetAdjacentNodes(nodeId);

    for (auto childNodeId : children)
    {
        if (IsNodeReady(childNodeId))
        {
            // If the node is already considered for update, don't add it again to the update queue
            if (find(updateQ._Get_container().begin(),
                updateQ._Get_container().end(),
                childNodeId) == updateQ._Get_container().end())
            {
                LogInfo("node[%d] is adding node[%d] to update Q", nodeId, childNodeId);
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

        for (auto currChildNodeId : currChildren)
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

    for (auto currChild : currChildren)
    {
        m_pOlcbpPlan->RemoveEdge(nodeId, currChild);
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::ComputeNodesWaitOnParentsCount()
{
    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeSet visitedNodes;

    _ASSERTE(m_pOlcbpPlan->Size() > 0);
    _ASSERTE(m_planRootNodeId != IOlcbpPlan::NullNodeID);

    // 1. Reset the NotReadyParentsCount field of each node
    //
    IOlcbpPlan::NodeSet planNodes;

    planNodes = m_pOlcbpPlan->GetNodes();
    for (auto nodeId : planNodes)
    {
        GetNodeData(nodeId).SetWaitOnParentsCount(0);
    }

    Q.push(m_planRootNodeId);
    visitedNodes.insert(m_planRootNodeId);

    // 2. Do a BFS on the plan graph and for closed node
    // increment the NotReadyParentsCount for child node
    //
    while (!Q.empty())
    {
        IOlcbpPlan::NodeID currNodeId = Q.front();
        Q.pop();

        const IOlcbpPlan::NodeSet& currChildren = m_pOlcbpPlan->GetAdjacentNodes(currNodeId);

        //bool waitOnCurrNode = m_pOlcbpPlan->GetNode(currNodeId)->State() != ESTATE_Succeeded;
        /*(IsActionNode(currNodeId) && m_pOlcbpPlan->GetNode(currNodeId)->State() != ESTATE_Succeeded) ||
        (IsGoalNode(currNodeId) && IsNodeOpen(currNodeId));*/

        for (auto currChildNodeId : currChildren)
        {
            if (IsActionNode(currNodeId) && m_pOlcbpPlan->GetNode(currNodeId)->State() != ESTATE_Succeeded)
                GetNodeData(currChildNodeId).IncWaitOnParentsCount();
            else if (IsGoalNode(currNodeId))
            {
                if (GetNodeData(currChildNodeId).SatisfyingGoal == currNodeId)
                {
                    if (IsNodeOpen(currNodeId))
                        GetNodeData(currChildNodeId).IncWaitOnParentsCount();
                }
                else
                {
                    if (m_pOlcbpPlan->GetNode(currNodeId)->State() != ESTATE_Succeeded)
                        GetNodeData(currChildNodeId).IncWaitOnParentsCount();
                }
            }

            if (visitedNodes.count(currChildNodeId) == 0)
            {
                Q.push(currChildNodeId);
                visitedNodes.insert(currChildNodeId);
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::OnNodeDone(_In_ IOlcbpPlan::NodeID nodeId)
{
    IOlcbpPlan::NodeID satisfyingGoalNode = GetNodeData(nodeId).SatisfyingGoal;

    if (satisfyingGoalNode != IOlcbpPlan::NullNodeID)
    {
        LogInfo("On '%s' node[%d] DONE, notifying its satisfying goal '%s' node[%d]",
            m_pOlcbpPlan->GetNode(nodeId)->ToString().c_str(), nodeId, m_pOlcbpPlan->GetNode(satisfyingGoalNode)->ToString().c_str(), satisfyingGoalNode);

        _ASSERTE(GetNodeData(satisfyingGoalNode).WaitOnChildrenCount > 0);
        GetNodeData(satisfyingGoalNode).DecWaitOnChildrenCount();
    }
}