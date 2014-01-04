#ifndef ONLINEPLANEXPANSIONEXECUTIONEX_H
#include "OnlinePlanExpansionExecutionEx.h"
#endif
#ifndef CASEBASEDREASONEREX_H
#include "CaseBasedReasonerEx.h"
#endif
#ifndef WINWARGUSGOALEX_H
#include "WinWargusGoalEx.h"
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

OnlinePlanExpansionExecutionEx::OnlinePlanExpansionExecutionEx(GoalEx* p_initialGoal, CaseBasedReasonerEx *p_casedBasedReasoner)
    : EngineComponent("OnlinePlanner")
{
	_caseBasedReasoner = p_casedBasedReasoner;
	_planRoot = PlanGraphNode::CreatePlanRoot(p_initialGoal);
    _planGraph.insert(make_pair(_planRoot->Id(), _planRoot));

	g_MessagePump.RegisterForMessage(MSG_EntityCreate, this);
	g_MessagePump.RegisterForMessage(MSG_EntityDestroy, this);
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::Update(const WorldClock& p_clock)
{
	if (_planRoot)
	{
		UpdatePlan(_planRoot, p_clock);
	}
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::ExpandGoal(PlanGraphNode *p_pGoalNode, CaseEx *p_pCase)
{
	PlanGraph					*pSubPlanGraph = p_pCase->GetPlanGraph();
	vector<int>					subPlanGraphRoots = pSubPlanGraph->GetRoots();
	PlanGraphNode::List	    preExpansionGoalChildren(p_pGoalNode->BelongingSubPlanChildren().begin(), p_pGoalNode->BelongingSubPlanChildren().end());
	PlanGraphNode::List	    subPlanTreeNodes;
	vector<int>					planGraphRootIndicies(subPlanGraphRoots.begin(), subPlanGraphRoots.end());
	queue<int>					Q;
	vector<bool>				visited;
	unsigned					currentRootIdx;
	PlanGraphNode				*pCurrentRoot;

	// 1. Construct plan tree nodes of plan digraph
	for(unsigned i = 0; i < pSubPlanGraph->Size(); ++i)
	{
		PlanGraphNode *pNode = new PlanGraphNode(pSubPlanGraph->operator [](i)->Value(), p_pGoalNode);
		pNode->BelongingCase(p_pCase);
		subPlanTreeNodes.push_back(pNode);
        _planGraph.insert(make_pair(pNode->Id(), pNode));
	}
	p_pGoalNode->BelongingCase(p_pCase);

	visited.resize(pSubPlanGraph->Size(), false);
	p_pGoalNode->CrossUnlinkChildren();

	// 2. Link the sub plan tree roots with the expanded goal node
	for(unsigned rootIdx = 0; rootIdx < planGraphRootIndicies.size(); ++rootIdx)
	{
		currentRootIdx = planGraphRootIndicies[rootIdx];
		pCurrentRoot = subPlanTreeNodes[currentRootIdx];

		// Cross link the goal node with the sub plan roots
		p_pGoalNode->CrossLinkChild(pCurrentRoot);

		// Enqueue the roots in a queue to expand them
		Q.push(currentRootIdx);
		visited[currentRootIdx] = true;
	}

	// 3. Continue construction of the sub plan tree with the rest of the plan graph nodes
	int				currentNodeIdx;
	vector<int>		currentNodeChildrenIndicies;
	int				currentNodeChildIdx;
	PlanGraphNode	*pCurrentNode;
	PlanGraphNode	*pCurrentNodeChild;

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
	vector<int>		subPlanLeafIndicies;
	PlanGraphNode	*pCurrentPreExpansionGoalChild;
	PlanGraphNode	*pCurrentSubPlanLeaf;
	unsigned		subPlanLeafIdx;

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

    g_MessagePump.Send(new Message(0, MSG_PlanStructureChange));
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::UpdatePlan(PlanGraphNode* p_pPlanRoot, const WorldClock& p_clock)
{
	PlanGraphNode::Queue	Q;
	PlanGraphNode*			pCurrentNode;

	// Root goal destroyed we may have an empty case-base, or exhausted all cases and nothing succeeded
	if (p_pPlanRoot == nullptr || p_pPlanRoot->IsNull())
		return;

	if(p_pPlanRoot->Children().empty() &&
		!_triedCases.empty())
	{
		LogInfo("Plan root node has no children, clearing the tried-cases");
		_triedCases.clear();
	}

	Q.push(p_pPlanRoot);

	while(!Q.empty())
	{
		pCurrentNode = Q.front();
		Q.pop();

		if (pCurrentNode->Type() == PGNTYPE_Goal)
		{
			UpdateGoalNode(pCurrentNode, p_clock, Q);
		}
		else if (pCurrentNode->Type() == PGNTYPE_Action)
		{
			UpdateActionNode(pCurrentNode, p_clock, Q);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::NotifyChildrenForParentSuccess(PlanGraphNode* p_pNode)
{
	LogInfo("Notifying node '%s' children with their parent success", p_pNode->PlanStep()->ToString().c_str());

	for(size_t i = 0; i < p_pNode->Children().size(); ++i)
		p_pNode->Children().at(i)->NotifyParentSuccess(p_pNode);
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::OnlinePlanExpansionExecutionEx::MarkCaseAsTried(PlanGraphNode* p_pStep, CaseEx* p_pCase)
{
	assert(_triedCases[p_pStep].find(p_pCase) == _triedCases[p_pStep].end());

	LogInfo("Marking case '%s'@%x as tried case for node %x", p_pCase->Goal()->ToString().c_str(), (void*)p_pCase, (void*)p_pStep);

	_triedCases[p_pStep].insert(p_pCase);
}
//////////////////////////////////////////////////////////////////////////
bool IStrategizer::OnlinePlanExpansionExecutionEx::IsCaseTried(PlanGraphNode* p_pStep, CaseEx* p_pCase)
{
	return _triedCases[p_pStep].find(p_pCase) != _triedCases[p_pStep].end();
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::OnlinePlanExpansionExecutionEx::UpdateGoalNode(PlanGraphNode* p_pCurrentNode, const WorldClock& p_clock, PlanGraphNode::Queue& p_updateQ)
{
	assert(p_pCurrentNode);
	PlanStepEx*		pCurrentPlanStep = p_pCurrentNode->PlanStep();
	bool			hasPreviousPlan = false;

	if (p_pCurrentNode->IsOpen())
	{
		hasPreviousPlan = DestroyGoalPlanIfExist(p_pCurrentNode);

		// The goal was previously expanded with a plan, but it somehow failed
		// Thats why it is now open
		// Revise the node belonging case as failed case
		if (hasPreviousPlan)
		{
			LogInfo("Node with plan-step '%s' is open and has children nodes, case is sent for revision and children have been destroyed", p_pCurrentNode->PlanStep()->ToString().c_str());
			_caseBasedReasoner->Reviser()->Revise(p_pCurrentNode->BelongingCase(), false);
		}

		CaseEx* caseEx = _caseBasedReasoner->Retriever()->Retrieve(p_pCurrentNode->GetGoal(), g_Game->Self()->State());

		if (caseEx != nullptr &&
			!IsCaseTried(p_pCurrentNode, caseEx))
		{
			LogInfo("Retrieved case '%s' has not been tried before, and its goal is being sent for expansion",
				caseEx->Goal()->ToString().c_str());

			MarkCaseAsTried(p_pCurrentNode, caseEx);
			ExpandGoal(p_pCurrentNode, caseEx);

			p_pCurrentNode->Close();

			NotifyChildrenForParentSuccess(p_pCurrentNode);
		}
		else
		{
			// The planner exhausted all possible plans form the case-base and nothing can succeed
			// We surrender!!
			if (p_pCurrentNode->SubPlanGoal()->IsNull())
			{
				LogWarning("Planner has exhausted all possible cases");
				_planRoot = nullptr;
				return;
			}
			else
				p_pCurrentNode->SubPlanGoal()->Open();
		}
	}
	else
	{
		if (pCurrentPlanStep->State() == ESTATE_Failed)
		{
			p_pCurrentNode->Open();
		}
		else
		{
			assert(pCurrentPlanStep->State() == ESTATE_Pending ||
				pCurrentPlanStep->State() == ESTATE_Succeeded ||
				pCurrentPlanStep->State() == ESTATE_END);

			pCurrentPlanStep->Update(p_clock);
			ConsiderReadyChildrenForUpdate(p_pCurrentNode, p_updateQ);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void IStrategizer::OnlinePlanExpansionExecutionEx::UpdateActionNode(PlanGraphNode* p_pCurrentNode, const WorldClock& p_clock, PlanGraphNode::Queue& p_updateQ)
{
	assert(p_pCurrentNode);
	PlanStepEx *pCurrentPlanStep  = p_pCurrentNode->PlanStep();
	assert(p_pCurrentNode->IsReady());

	if (p_pCurrentNode->IsOpen())
	{
		if (pCurrentPlanStep->State() == ESTATE_Failed)
		{
			// We check to make sure that our parent is open before closing
			// Our sub plan goal may be already open because any of our siblings may have failed and opened our sub plan goal
			if (!p_pCurrentNode->SubPlanGoal()->IsOpen())
				p_pCurrentNode->SubPlanGoal()->Open();
		}
		else if (pCurrentPlanStep->State() == ESTATE_Succeeded)
		{
			p_pCurrentNode->Close();
			NotifyChildrenForParentSuccess(p_pCurrentNode);
		}
		else
		{
			assert(pCurrentPlanStep->State() == ESTATE_NotPrepared ||
				pCurrentPlanStep->State() == ESTATE_Pending ||
				pCurrentPlanStep->State() == ESTATE_Executing ||
				pCurrentPlanStep->State() == ESTATE_END);

			pCurrentPlanStep->Update(p_clock);
		}
	}
	else
	{
		assert(pCurrentPlanStep->State() == ESTATE_Succeeded);

		ConsiderReadyChildrenForUpdate(p_pCurrentNode, p_updateQ);
	}
}

//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::NotifyMessegeSent(Message* p_message)
{
	queue<PlanGraphNode*>	Q;
	PlanGraphNode*			pCurrentPlanStep;
	bool					dummy = false;

	if (!_planRoot)
		return;

	Q.push(_planRoot);

	while(!Q.empty())
	{
		pCurrentPlanStep = Q.front();
		Q.pop();

		pCurrentPlanStep->PlanStep()->HandleMessage(p_message, dummy);
		// Obsolete parameter: No one should use the message consuming anymore
		assert(dummy == false);

		ConsiderReadyChildrenForUpdate(pCurrentPlanStep, Q);
	}
}
//////////////////////////////////////////////////////////////////////////
bool OnlinePlanExpansionExecutionEx::DestroyGoalPlanIfExist(PlanGraphNode* p_pPlanGoalNode)
{
	PlanGraphNode::Queue	Q;
	PlanGraphNode::Set		visitedNodes;
	PlanGraphNode*			pCurrentNode;
	PlanGraphNode::List	children(p_pPlanGoalNode->Children());
	PlanGraphNode::List	currentNodeChildren;
	PlanGraphNode::List	preExpansionChildren(p_pPlanGoalNode->BelongingSubPlanChildren());

	// Unlink sub plan roots
	for (PlanGraphNode::List::const_iterator itr = children.begin();
		itr != children.end(); ++itr)
	{
		// The current child is a result of expanding p_pPlanGoalNode before
		// Where p_pPlanGoalNode is the current node sub plan goal
		if ((*itr)->SubPlanGoal() == p_pPlanGoalNode)
		{
			Q.push(*itr);
			visitedNodes.insert(*itr);
			p_pPlanGoalNode->CrossUnlinkChild(*itr);
		}
	}

	// We don't have a sub plan attached to p_pPlanGoalNode
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
			// Those when p_pPlanGoalNode was originally expanded
			DestroyGoalPlanIfExist(pCurrentNode);
		}

		if (!pCurrentNode->Children().empty())
		{
			currentNodeChildren = pCurrentNode->Children();

			for (PlanGraphNode::List::const_iterator itr = currentNodeChildren.begin();
				itr != currentNodeChildren.end(); ++itr)
			{
				// Nodes from the original sub plan (as appeared in the case plan graph) should not be considered from unlinking from their children
				if ((*itr)->SubPlanGoal() == p_pPlanGoalNode &&
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
    assert(_planGraph.count(p_pPlanGoalNode->Id()) > 0);
    _planGraph.erase(p_pPlanGoalNode->Id());

	assert(p_pPlanGoalNode->Children().empty());

	// Cross link the old pre-expansion children with the plan goal node
	for (PlanGraphNode::List::const_iterator itr = preExpansionChildren.begin();
		itr != preExpansionChildren.end(); ++itr)
	{
		p_pPlanGoalNode->CrossLinkChild(*itr);
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::ConsiderReadyChildrenForUpdate(PlanGraphNode* p_pNode, PlanGraphNode::Queue &p_updateQueue)
{
	const PlanGraphNode::List& children =  p_pNode->Children();

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