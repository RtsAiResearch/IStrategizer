#ifndef ONLINEPLANEXPANSIONEXECUTIONEX_H
#include "OnlinePlanExpansionExecutionEx.h"
#endif
#ifndef CASEBASEDREASONEREX_H
#include "CaseBasedReasonerEx.h"
#endif
#ifndef WINGAMEGOAL_H
#include "WinGameGoal.h"
#endif
#ifndef PLANTREENODEEX_H
#include "PlanTreeNodeEx.h"
#endif
#ifndef CASEEX_H
#include "CaseEx.h"
#endif
#ifndef RETRIEVEREX_H
#include "RetrieverEx.h"
#endif
#ifndef RTSGAME_H
#include "RtsGame.h"
#endif
#ifndef GAMEPLAYER_H
#include "GamePlayer.h"
#endif
#ifndef ACTION_H
#include "Action.h"
#endif
#ifndef ATTACKGROUNDACTION_H
#include "AttackGroundAction.h"
#endif
#ifndef MESSAGEPUMP_H
#include "MessagePump.h"
#endif
#ifndef MESSAGE_H
#include "Message.h"
#endif
#include "GameStateEx.h"
#include "AbstractReviser.h"
#include "Toolbox.h"
#include <cstdio>
#include <bitset>
#include "Logger.h"

using namespace std;
using namespace IStrategizer;

OnlinePlanExpansionExecutionEx::OnlinePlanExpansionExecutionEx(RtsGame& p_RtsGame, GoalEx* p_initialGoal, CaseBasedReasonerEx *p_casedBasedReasoner)
    : EngineComponent(p_RtsGame, "OnlinePlanner")
{
    _caseBasedReasoner = p_casedBasedReasoner;
    _planRoot = PlanGraphNode::CreatePlanRoot(p_initialGoal);
    _planGraph.insert(make_pair(_planRoot->Id(), _planRoot));

    MessagePump::Instance(p_RtsGame).RegisterForMessage(p_RtsGame, MSG_EntityCreate, this);
    MessagePump::Instance(p_RtsGame).RegisterForMessage(p_RtsGame, MSG_EntityDestroy, this);
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::Update(RtsGame& p_RtsGame, const WorldClock& p_clock)
{
    if (_planRoot)
    {
        UpdatePlan(p_RtsGame, _planRoot, p_clock);
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::ExpandGoal(RtsGame& p_RtsGame, PlanGraphNode *p_GoalNode, CaseEx *p_Case)
{
    PlanGraph                    *pSubPlanGraph = p_Case->GetPlanGraph();
    vector<int>                    subPlanGraphRoots = pSubPlanGraph->GetRoots();
    PlanGraphNode::List        preExpansionGoalChildren(p_GoalNode->BelongingSubPlanChildren().begin(), p_GoalNode->BelongingSubPlanChildren().end());
    PlanGraphNode::List        subPlanTreeNodes;
    vector<int>                    planGraphRootIndicies(subPlanGraphRoots.begin(), subPlanGraphRoots.end());
    queue<int>                    Q;
    vector<bool>                visited;
    unsigned                    currentRootIdx;
    PlanGraphNode                *pCurrentRoot;

    // 1. Construct plan tree nodes of plan digraph
    for(unsigned i = 0; i < pSubPlanGraph->Size(); ++i)
    {
        PlanGraphNode *pNode = new PlanGraphNode(pSubPlanGraph->operator [](i)->Value(), p_GoalNode);
        pNode->BelongingCase(p_Case);
        subPlanTreeNodes.push_back(pNode);
        _planGraph.insert(make_pair(pNode->Id(), pNode));
    }
    p_GoalNode->BelongingCase(p_Case);

    visited.resize(pSubPlanGraph->Size(), false);
    p_GoalNode->CrossUnlinkChildren();

    // 2. Link the sub plan tree roots with the expanded goal node
    for(unsigned rootIdx = 0; rootIdx < planGraphRootIndicies.size(); ++rootIdx)
    {
        currentRootIdx = planGraphRootIndicies[rootIdx];
        pCurrentRoot = subPlanTreeNodes[currentRootIdx];

        // Cross link the goal node with the sub plan roots
        p_GoalNode->CrossLinkChild(pCurrentRoot);

        // Enqueue the roots in a queue to expand them
        Q.push(currentRootIdx);
        visited[currentRootIdx] = true;
    }

    // 3. Continue construction of the sub plan tree with the rest of the plan graph nodes
    int                currentNodeIdx;
    vector<int>        currentNodeChildrenIndicies;
    int                currentNodeChildIdx;
    PlanGraphNode    *pCurrentNode;
    PlanGraphNode    *pCurrentNodeChild;

    while(!Q.empty())
    {
        currentNodeIdx = Q.front();
        Q.pop();

        pCurrentNode = subPlanTreeNodes[currentNodeIdx];
        currentNodeChildrenIndicies = pSubPlanGraph->GetChildren(currentNodeIdx);

        for(unsigned i = 0; i < currentNodeChildrenIndicies.size(); ++i)
        {
            currentNodeChildIdx= currentNodeChildrenIndicies[i];
            pCurrentNodeChild = subPlanTreeNodes[currentNodeChildIdx];

            // If my child not visited, consider it for expansion
            if(!visited[currentNodeChildIdx])
            {
                Q.push(currentNodeChildIdx);
                visited[currentNodeChildIdx] = true;
            }

            // Cross link the current node with its child
            pCurrentNode->CrossLinkChild(pCurrentNodeChild);
        }

        // The current children are as they appear in the case before my expansion if I am a goal
        pCurrentNode->SetChildrenAsBelongingSubPlanChildren();
    }

    // 4. Link the new sub plan subPlanLeafIndicies with the goal subPlanLeafIndicies before expansion
    vector<int>        subPlanLeafIndicies;
    PlanGraphNode    *pCurrentPreExpansionGoalChild;
    PlanGraphNode    *pCurrentSubPlanLeaf;
    unsigned        subPlanLeafIdx;

    // If the goal had no children before expansion, then there is nothing to link and we are done
    if(!preExpansionGoalChildren.empty())
    {
        subPlanLeafIndicies = pSubPlanGraph->GetLeaves();

        for(unsigned preExpansionGoaldChildIdx = 0;
            preExpansionGoaldChildIdx < preExpansionGoalChildren.size();
            ++preExpansionGoaldChildIdx)
        {
            pCurrentPreExpansionGoalChild = preExpansionGoalChildren[preExpansionGoaldChildIdx];

            for (unsigned i = 0; i < subPlanLeafIndicies.size(); ++i)
            {
                subPlanLeafIdx = subPlanLeafIndicies[i];
                pCurrentSubPlanLeaf = subPlanTreeNodes[subPlanLeafIdx];

                // Cross link the current pre-expansion leaf with sub plan leaf
                pCurrentSubPlanLeaf->CrossLinkChild(pCurrentPreExpansionGoalChild);
            }
        }
    }

    MessagePump::Instance(p_RtsGame).Send(new Message(0, MSG_PlanStructureChange));
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::UpdatePlan(RtsGame& p_RtsGame, PlanGraphNode* p_PlanRoot, const WorldClock& p_clock)
{
    PlanGraphNode::Queue    Q;
    PlanGraphNode*            pCurrentNode;

    // Root goal destroyed we may have an empty case-base, or exhausted all cases and nothing succeeded
    if (p_PlanRoot == nullptr || p_PlanRoot->IsNull())
        return;

    if(p_PlanRoot->Children().empty() &&
        !_triedCases.empty())
    {
        LogInfo("Plan root node has no children, clearing the tried-cases");
        _triedCases.clear();
    }

    Q.push(p_PlanRoot);

    while(!Q.empty())
    {
        pCurrentNode = Q.front();
        Q.pop();

        if (pCurrentNode->Type() == PGNTYPE_Goal)
        {
            UpdateGoalNode(p_RtsGame, pCurrentNode, p_clock, Q);
        }
        else if (pCurrentNode->Type() == PGNTYPE_Action)
        {
            UpdateActionNode(p_RtsGame, pCurrentNode, p_clock, Q);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::NotifyChildrenForParentSuccess(PlanGraphNode* p_Node)
{
    LogInfo("Notifying node '%s' children with their parent success", p_Node->PlanStep()->ToString().c_str());

    for(size_t i = 0; i < p_Node->Children().size(); ++i)
        p_Node->Children().at(i)->NotifyParentSuccess(p_Node);
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::OnlinePlanExpansionExecutionEx::MarkCaseAsTried(PlanGraphNode* p_Step, CaseEx* p_Case)
{
    assert(_triedCases[p_Step].find(p_Case) == _triedCases[p_Step].end());

    LogInfo("Marking case '%s'@%x as tried case for node %x", p_Case->Goal()->ToString().c_str(), (void*)p_Case, (void*)p_Step);

    _triedCases[p_Step].insert(p_Case);
}
//////////////////////////////////////////////////////////////////////////
bool IStrategizer::OnlinePlanExpansionExecutionEx::IsCaseTried(PlanGraphNode* p_Step, CaseEx* p_Case)
{
    return _triedCases[p_Step].find(p_Case) != _triedCases[p_Step].end();
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::OnlinePlanExpansionExecutionEx::UpdateGoalNode(RtsGame& p_RtsGame, PlanGraphNode* p_CurrentNode, const WorldClock& p_clock, PlanGraphNode::Queue& p_updateQ)
{
    assert(p_CurrentNode);
    PlanStepEx*        pCurrentPlanStep = p_CurrentNode->PlanStep();
    bool            hasPreviousPlan = false;

    if (p_CurrentNode->IsOpen())
    {
        hasPreviousPlan = DestroyGoalPlanIfExist(p_CurrentNode);

        // The goal was previously expanded with a plan, but it somehow failed
        // Thats why it is now open
        // Revise the node belonging case as failed case
        if (hasPreviousPlan)
        {
            LogInfo("Node with plan-step '%s' is open and has children nodes, case is sent for revision and children have been destroyed", p_CurrentNode->PlanStep()->ToString().c_str());
            _caseBasedReasoner->Reviser()->Revise(p_CurrentNode->BelongingCase(), false);
        }

        CaseEx* caseEx = _caseBasedReasoner->Retriever()->Retrieve(p_CurrentNode->GetGoal(), p_RtsGame.Self()->State());

        if (caseEx != nullptr &&
            !IsCaseTried(p_CurrentNode, caseEx))
        {
            LogInfo("Retrieved case '%s' has not been tried before, and its goal is being sent for expansion",
                caseEx->Goal()->ToString().c_str());

            MarkCaseAsTried(p_CurrentNode, caseEx);
            ExpandGoal(p_RtsGame, p_CurrentNode, caseEx);

            p_CurrentNode->Close();

            NotifyChildrenForParentSuccess(p_CurrentNode);
        }
        else
        {
            // The planner exhausted all possible plans form the case-base and nothing can succeed
            // We surrender!!
            if (p_CurrentNode->SubPlanGoal()->IsNull())
            {
                LogWarning("Planner has exhausted all possible cases");
                _planRoot = nullptr;
                return;
            }
            else
                p_CurrentNode->SubPlanGoal()->Open();
        }
    }
    else
    {
        if (pCurrentPlanStep->State() == ESTATE_Failed)
        {
            p_CurrentNode->Open();
        }
        else
        {
            assert(pCurrentPlanStep->State() == ESTATE_Pending ||
                pCurrentPlanStep->State() == ESTATE_Succeeded ||
                pCurrentPlanStep->State() == ESTATE_END);

            pCurrentPlanStep->Update(p_RtsGame, p_clock);
            ConsiderReadyChildrenForUpdate(p_CurrentNode, p_updateQ);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::OnlinePlanExpansionExecutionEx::UpdateActionNode(RtsGame& p_RtsGame, PlanGraphNode* p_CurrentNode, const WorldClock& p_clock, PlanGraphNode::Queue& p_updateQ)
{
    assert(p_CurrentNode);
    PlanStepEx *pCurrentPlanStep  = p_CurrentNode->PlanStep();
    assert(p_CurrentNode->IsReady());

    if (p_CurrentNode->IsOpen())
    {
        if (pCurrentPlanStep->State() == ESTATE_Failed)
        {
            // We check to make sure that our parent is open before closing
            // Our sub plan goal may be already open because any of our siblings may have failed and opened our sub plan goal
            if (!p_CurrentNode->SubPlanGoal()->IsOpen())
                p_CurrentNode->SubPlanGoal()->Open();
        }
        else if (pCurrentPlanStep->State() == ESTATE_Succeeded)
        {
            p_CurrentNode->Close();
            NotifyChildrenForParentSuccess(p_CurrentNode);
        }
        else
        {
            assert(pCurrentPlanStep->State() == ESTATE_NotPrepared ||
                pCurrentPlanStep->State() == ESTATE_Pending ||
                pCurrentPlanStep->State() == ESTATE_Executing ||
                pCurrentPlanStep->State() == ESTATE_END);

            pCurrentPlanStep->Update(p_RtsGame, p_clock);
        }
    }
    else
    {
        assert(pCurrentPlanStep->State() == ESTATE_Succeeded);

        ConsiderReadyChildrenForUpdate(p_CurrentNode, p_updateQ);
    }
}

//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::NotifyMessegeSent(RtsGame& p_RtsGame, Message* p_message)
{
    queue<PlanGraphNode*>    Q;
    PlanGraphNode*            pCurrentPlanStep;
    bool                    dummy = false;

    if (!_planRoot)
        return;

    Q.push(_planRoot);

    while(!Q.empty())
    {
        pCurrentPlanStep = Q.front();
        Q.pop();

        pCurrentPlanStep->PlanStep()->HandleMessage(p_RtsGame, p_message, dummy);
        // Obsolete parameter: No one should use the message consuming anymore
        assert(dummy == false);

        ConsiderReadyChildrenForUpdate(pCurrentPlanStep, Q);
    }
}
//////////////////////////////////////////////////////////////////////////
bool OnlinePlanExpansionExecutionEx::DestroyGoalPlanIfExist(PlanGraphNode* p_PlanGoalNode)
{
    PlanGraphNode::Queue    Q;
    PlanGraphNode::Set        visitedNodes;
    PlanGraphNode*            pCurrentNode;
    PlanGraphNode::List    children(p_PlanGoalNode->Children());
    PlanGraphNode::List    currentNodeChildren;
    PlanGraphNode::List    preExpansionChildren(p_PlanGoalNode->BelongingSubPlanChildren());

    // Unlink sub plan roots
    for (PlanGraphNode::List::const_iterator itr = children.begin();
        itr != children.end(); ++itr)
    {
        // The current child is a result of expanding p_PlanGoalNode before
        // Where p_PlanGoalNode is the current node sub plan goal
        if ((*itr)->SubPlanGoal() == p_PlanGoalNode)
        {
            Q.push(*itr);
            visitedNodes.insert(*itr);
            p_PlanGoalNode->CrossUnlinkChild(*itr);
        }
    }

    // We don't have a sub plan attached to p_PlanGoalNode
    // There is nothing to destroy
    if (Q.empty())
        return false;

    // Unlink the rest of the sub plan nodes
    while (!Q.empty())
    {
        pCurrentNode = Q.front();
        Q.pop();

        if (pCurrentNode->Type() == PGNTYPE_Goal)
        {
            // If I am a goal then recursively destroy my sub-plan
            // After the recursive destroy is done, pCurrentNode children should be
            // Those when p_PlanGoalNode was originally expanded
            DestroyGoalPlanIfExist(pCurrentNode);
        }

        if (!pCurrentNode->Children().empty())
        {
            currentNodeChildren = pCurrentNode->Children();

            for (PlanGraphNode::List::const_iterator itr = currentNodeChildren.begin();
                itr != currentNodeChildren.end(); ++itr)
            {
                // Nodes from the original sub plan (as appeared in the case plan graph) should not be considered from unlinking from their children
                if ((*itr)->SubPlanGoal() == p_PlanGoalNode &&
                    visitedNodes.find(*itr) == visitedNodes.end())
                {
                    Q.push(*itr);
                    visitedNodes.insert(*itr);
                }

                pCurrentNode->CrossUnlinkChild(*itr);
            }
        }
        
        // Consider nodes memory management in the "parallel" plan
        // delete pCurrentNode;
    }

    // Remove node from graph
    assert(_planGraph.count(p_PlanGoalNode->Id()) > 0);
    _planGraph.erase(p_PlanGoalNode->Id());

    assert(p_PlanGoalNode->Children().empty());

    // Cross link the old pre-expansion children with the plan goal node
    for (PlanGraphNode::List::const_iterator itr = preExpansionChildren.begin();
        itr != preExpansionChildren.end(); ++itr)
    {
        p_PlanGoalNode->CrossLinkChild(*itr);
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::ConsiderReadyChildrenForUpdate(PlanGraphNode* p_Node, PlanGraphNode::Queue &p_updateQueue)
{
    const PlanGraphNode::List& children =  p_Node->Children();

    for(size_t i = 0; i < children.size(); i++)
    {
        if (children[i]->IsReady())
        {
            // If the node is already considered for update, don't add it again to the update queue
            if (find(p_updateQueue._Get_container().begin(),
                p_updateQueue._Get_container().end(),
                children[i]) == p_updateQueue._Get_container().end())
            {
                p_updateQueue.push(children[i]);
            }
        }
    }
}
