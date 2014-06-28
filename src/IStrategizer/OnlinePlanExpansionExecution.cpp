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

    // 4. Since the plan structure changed, recompute the NotReadyParentsCount
    // for plan graph node
    ComputeNodesWaitOnParentsCount();

    // 5. Record that the expanded goal node is waiting for N number of open nodes
    // If the number of those open nodes = 0, and the goal node is not satisfied, then
    // there is no way for the goal to succeed, and the goal should fail
    GetNodeData(expansionGoalNodeId).SetWaitOnChildrenCount(pExpansionCase->Plan()->Size());

    m_planStructureChangedThisFrame = true;

    LogInfo("Plan after node[%d] expansion %s", expansionGoalNodeId, m_pOlcbpPlan->ToString().c_str());
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::Update(_In_ const WorldClock& clock)
{
    m_pOlcbpPlan->Lock();

    // LogInfo("### START PLAN UPDATE ###");

    // We have exhausted all possible plans. We have surrendered, nothing to do
    if (m_pOlcbpPlan->Size() > 0)
    {
        IOlcbpPlan::NodeQueue Q;
        IOlcbpPlan::NodeID currentNode;
        IOlcbpPlan::NodeSerializedSet planRoots;

        Q.push(m_planRootNodeId);

        while(!Q.empty())
        {
            currentNode = Q.front();
            Q.pop();

            if (!m_pOlcbpPlan->Contains(currentNode))
            {
                LogWarning("A non existing node was there in the update queue, skipping it");
                continue;
            }

            if (IsGoalNode(currentNode))
            {
                if (!IsNodeDone(currentNode))
                    UpdateGoalNode(currentNode, clock);

                AddReadyChildrenToUpdateQueue(currentNode, Q);
            }
            else if (IsActionNode(currentNode))
            {
                UpdateActionNode(currentNode, clock, Q);
            }
        }
    }
    else
    {
        // Clear the used data structures
        m_nodeData.clear();
        m_clonedNodesMapping.clear();

        // Create the initial goal
        AbstractRetriever::RetrieveOptions options;
        options.GoalTypeId = m_rootGoalType;
        options.pGameState = g_Game;
        options.Exclusions = m_rootNodeExclusions;
        CaseEx* pCandidateCase = m_pCbReasoner->Retriever()->Retrieve(options);
        m_rootNodeExclusions.insert(pCandidateCase);
        PlanStepEx* pRootNode = pCandidateCase->Goal();
        m_planRootNodeId = m_pOlcbpPlan->AddNode(pRootNode, pRootNode->Id());
        m_nodeData[m_planRootNodeId] = OlcbpPlanNodeData();
        OpenNode(m_planRootNodeId);
    }

    // LogInfo("### END PLAN UPDATE ###");

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
                // _ASSERTE(GetNodeData(childId).WaitOnParentsCount > 0);
                if (GetNodeData(childId).WaitOnParentsCount > 0)
                {
                    LogInfo("Update node '%s' with parent readiness", m_pOlcbpPlan->GetNode(childId)->ToString().c_str());
                    GetNodeData(childId).DecWaitOnParentsCount();
                }
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::UpdateGoalPlanChildrenWithParentReadiness(_In_ IOlcbpPlan::NodeID nodeId)
{
    _ASSERTE(IsGoalNode(nodeId));
    LogInfo("Updating node '%s' children with their parent readiness", m_pOlcbpPlan->GetNode(nodeId)->ToString().c_str());

    const IOlcbpPlan::NodeSerializedSet& children =  m_pOlcbpPlan->GetAdjacentNodes(nodeId);

    for (auto childId : children)
    {
        _ASSERTE(GetNodeData(childId).WaitOnParentsCount > 0);

        if (GetNodeData(childId).SatisfyingGoal == nodeId)
        {
            LogInfo("Update node '%s' with parent readiness", m_pOlcbpPlan->GetNode(childId)->ToString().c_str());
            GetNodeData(childId).DecWaitOnParentsCount();
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
void IStrategizer::OnlinePlanExpansionExecution::UpdateGoalNode(_In_ IOlcbpPlan::NodeID currentNode, _In_ const WorldClock& clock)
{
    GoalEx* pCurrentGoalNode = (GoalEx*)m_pOlcbpPlan->GetNode(currentNode);

#pragma region Node Open
    if (IsNodeOpen(currentNode))
    {
        _ASSERTE(pCurrentGoalNode->State() == ESTATE_NotPrepared);
        _ASSERTE(IsNodeDone(currentNode) == false);

        bool hasPreviousPlan = DestroyGoalPlanIfExist(currentNode);

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
            m_pCbReasoner->Retainer()->Flush();
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
            CaseSet exclusions = GetNodeData(currentNode).TriedCases;

            IOlcbpPlan::NodeID satisfyingGoalNode = GetNodeData(currentNode).SatisfyingGoal;

            if (satisfyingGoalNode != IOlcbpPlan::NullNodeID)
            {
                LogInfo("Excluding satisfying goal node %s to avoid recursive plan expansion", m_pOlcbpPlan->GetNode(satisfyingGoalNode)->ToString().c_str());
                // Add belonging case to exclusion to avoid recursive expansion of plans
                _ASSERTE(GetNodeData(satisfyingGoalNode).BelongingCase != nullptr);
                exclusions.insert(GetNodeData(satisfyingGoalNode).BelongingCase);
            }

            pCurrentGoalNode->AdaptParameters(*g_Game);
            AbstractRetriever::RetrieveOptions options;
            options.GoalTypeId = (GoalType)pCurrentGoalNode->StepTypeId();
            options.pGameState = g_Game;
            options.Exclusions = exclusions;
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
        if (pCurrentGoalNode->SuccessConditionsSatisfied(*g_Game))
        {
            _ASSERTE(pCurrentGoalNode->State() == ESTATE_NotPrepared);
            pCurrentGoalNode->State(ESTATE_Succeeded, *g_Game, clock);
            OnGoalNodeSucceeded(currentNode);
        }
        else
        {
            _ASSERTE(pCurrentGoalNode->State() == ESTATE_NotPrepared);

            // The goal is not done yet, and all of its children are done and
            // finished execution. It does not make sense for the goal to continue
            // this goal should fail, it has no future
            if (GetNodeData(currentNode).WaitOnChildrenCount == 0)
            {
                LogInfo("Goal '%s' is still not done and all of its children are done execution, failing it", 
                    pCurrentGoalNode->ToString().c_str());
                pCurrentGoalNode->State(ESTATE_NotPrepared, *g_Game, clock);
                OpenNode(currentNode);
            }
        }
    }
#pragma endregion
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
        //    // We check to make sure that our parent is open before closing
        //    // Our sub plan goal may be already open because any of our siblings may have failed and opened our sub plan goal
        //    if (!IsNodeOpen(GetNodeData(currentNode).SatisfyingGoal))
        //        OpenNode(GetNodeData(currentNode).SatisfyingGoal);
    }
    else if (pCurrentPlanStep->State() == ESTATE_Succeeded)
    {
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
bool OnlinePlanExpansionExecution::DestroyGoalPlanIfExist(_In_ IOlcbpPlan::NodeID planGoalNodeId)
{
    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeSerializedSet visitedNodes;

    // 1. Do a BFS to collect the pre-expansion children and the goal subplan nodes
    // The visited nodes in the traversal are the goal subplan nodes
    for (auto childNode : m_pOlcbpPlan->GetAdjacentNodes(planGoalNodeId))
    {
        if (GetNodeData(childNode).SatisfyingGoal == planGoalNodeId)
        {
            Q.push(childNode);
            visitedNodes.insert(childNode);
        }
    }

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

    // 4. Since there is no plan destroyed, fast return with false
    if (visitedNodes.empty())
        return false;
    else
    {
        // 4. Since the plan structure changed, recompute the WaitOnParentsCount
        // for plan graph node
        ComputeNodesWaitOnParentsCount();
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
void OnlinePlanExpansionExecution::GetNodeChildrenInBelongingSubplan(_In_ IOlcbpPlan::NodeID nodeId, _Out_ IOlcbpPlan::NodeSerializedSet& children) const
{
    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeSerializedSet visitedNodes;
    IOlcbpPlan::NodeID nodeSatisfyingGoal = GetNodeData(nodeId).SatisfyingGoal;

    Q.push(nodeId);
    visitedNodes.insert(nodeId);

    while (!Q.empty())
    {
        IOlcbpPlan::NodeID currNodeId = Q.front();
        Q.pop();

        const IOlcbpPlan::NodeSerializedSet& currChildren = m_pOlcbpPlan->GetAdjacentNodes(currNodeId);

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
    IOlcbpPlan::NodeSerializedSet currChildren = m_pOlcbpPlan->GetAdjacentNodes(nodeId);

    for (auto currChild : currChildren)
    {
        m_pOlcbpPlan->RemoveEdge(nodeId, currChild);
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecution::ComputeNodesWaitOnParentsCount()
{
    LogInfo("Computing graph nodes WaitOnParentsCount property");

    IOlcbpPlan::NodeQueue Q;
    IOlcbpPlan::NodeSerializedSet visitedNodes;

    _ASSERTE(m_pOlcbpPlan->Size() > 0);
    _ASSERTE(m_planRootNodeId != IOlcbpPlan::NullNodeID);

    // 1. Reset the NotReadyParentsCount field of each node
    //
    IOlcbpPlan::NodeSerializedSet planNodes;

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

        const IOlcbpPlan::NodeSerializedSet& currChildren = m_pOlcbpPlan->GetAdjacentNodes(currNodeId);

        LogInfo("Computing WaitOnParentsCount for node[%d] children", currNodeId);

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
                    LogInfo("Node[%d] is from Node[%d] expanded case", currChildNodeId, currNodeId);

                    if (IsNodeOpen(currNodeId))
                        GetNodeData(currChildNodeId).IncWaitOnParentsCount();
                    else
                        LogInfo("Node[%d] was not considered since it is closed", currNodeId);
                }
                else
                {
                    LogInfo("Node[%d] is from the original plan Node[%d] belong to", currChildNodeId, currNodeId);

                    if (m_pOlcbpPlan->GetNode(currNodeId)->State() != ESTATE_Succeeded)
                        GetNodeData(currChildNodeId).IncWaitOnParentsCount();
                    else
                        LogInfo("Node[%d] was not considered since its parent Node[%d] is already succeeded", currChildNodeId, currNodeId);
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
        LogInfo("On '%s' DONE, notifying its satisfying goal '%s'",
            m_pOlcbpPlan->GetNode(nodeId)->ToString().c_str(), m_pOlcbpPlan->GetNode(satisfyingGoalNode)->ToString().c_str());

        _ASSERTE(GetNodeData(satisfyingGoalNode).WaitOnChildrenCount > 0);
        GetNodeData(satisfyingGoalNode).DecWaitOnChildrenCount();
    }

    if (IsGoalNode(nodeId))
    {
        GoalEx* pGoal = (GoalEx*)m_pOlcbpPlan->GetNode(nodeId);
        UnassignGoalType(pGoal->Key());
    }
}

void OnlinePlanExpansionExecution::OnGoalNodeSucceeded(_In_ IOlcbpPlan::NodeID nodeId)
{
    GoalEx* pGoal = (GoalEx*)m_pOlcbpPlan->GetNode(nodeId);
    _ASSERTE(pGoal->State() == ESTATE_Succeeded);

    if (GetNodeData(nodeId).BelongingCase != nullptr)
    {
        LogInfo("Goal=%s succeeded, revising and retaining it", pGoal->ToString().c_str());
        CaseEx* currentCase = GetLastCaseForGoalNode(nodeId);
        m_pCbReasoner->Reviser()->Revise(currentCase, true);
        UpdateHistory(currentCase);
        m_pCbReasoner->Retainer()->Retain(currentCase);
        m_pCbReasoner->Retainer()->Flush();
    }
    else
    {
        LogInfo("Goal=%s succeeded without the need to expand it, no need to revise it", pGoal->ToString().c_str());
    }

    UpdateBelongingSubplanChildrenWithParentReadiness(nodeId);

    if (m_planRootNodeId == nodeId)
    {
		g_MessagePump.Send(new DataMessage<IOlcbpPlan>(0, MSG_PlanComplete, nullptr));
		m_pOlcbpPlan->Clear();
	}
}

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
        m_pCbReasoner->Retainer()->Flush();
    }
}