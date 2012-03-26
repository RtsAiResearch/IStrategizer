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
#include "Toolbox.h"
#include <cstdio>
#include <bitset>

using namespace std;
using namespace IStrategizer;
using namespace OLCBP;


OnlinePlanExpansionExecutionEx::OnlinePlanExpansionExecutionEx(GoalEx* p_initialGoal, CaseBasedReasonerEx *p_casedBasedReasoner)
{
	_caseBasedReasoner = p_casedBasedReasoner;
	_planRoot = PlanTreeNodeEx::CreatePlanRoot(p_initialGoal);

	g_MessagePump.RegisterForMessage(MSG_EntityCreate, this);
	g_MessagePump.RegisterForMessage(MSG_EntityDestroy, this);
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::Update(unsigned long p_cycles)
{
	if (_planRoot)
	{
		UpdatePlan(_planRoot, p_cycles);
	}
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::ExpandGoal(PlanTreeNodeEx *p_pGoalNode, PlanGraph *p_pSubPlanGraph)
{
	vector<int>					subPlanGraphRoots = p_pSubPlanGraph->GetRoots();
	PlanTreeNodeEx::NodeList	preExpansionGoalChildren(p_pGoalNode->BelongingSubPlanChildren().begin(), p_pGoalNode->BelongingSubPlanChildren().end());
	PlanTreeNodeEx::NodeList	subPlanTreeNodes;
	vector<int>					planGraphRootIndicies(subPlanGraphRoots.begin(), subPlanGraphRoots.end());
	queue<int>					Q;
	vector<bool>				visited;
	unsigned					currentRootIdx;
	PlanTreeNodeEx				*pCurrentRoot;

	// 1. Construct plan tree nodes of plan digraph
	for(unsigned i = 0; i < p_pSubPlanGraph->Size(); ++i)
	{
		subPlanTreeNodes.push_back(new PlanTreeNodeEx(p_pSubPlanGraph->operator [](i)->Value(), p_pGoalNode));
	}

	visited.resize(p_pSubPlanGraph->Size(), false);
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
	PlanTreeNodeEx	*pCurrentNode;
	PlanTreeNodeEx	*pCurrentNodeChild;

	while(!Q.empty())
	{
		currentNodeIdx = Q.front();
		Q.pop();

		pCurrentNode = subPlanTreeNodes[currentNodeIdx];
		currentNodeChildrenIndicies = p_pSubPlanGraph->GetChildren(currentNodeIdx);

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
	vector<int>									subPlanLeafIndicies;
	PlanTreeNodeEx								*pCurrentPreExpansionGoalChild;
	PlanTreeNodeEx								*pCurrentSubPlanLeaf;
	unsigned									subPlanLeafIdx;

	// If the goal had no children before expansion, then there is nothing to link and we are done
	if(!preExpansionGoalChildren.empty())
	{
		subPlanLeafIndicies = p_pSubPlanGraph->GetLeaves();

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
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::UpdatePlan(PlanTreeNodeEx* p_pPlanRoot, unsigned long p_cycles)
{
	queue<PlanTreeNodeEx*>	Q;
	PlanTreeNodeEx*			pCurrentNode;
	PlanStepEx*				pCurrentPlanStep;

	// Root goal destroyed we may have an empty case-base, or exhausted all cases and nothing succeeded
	if (p_pPlanRoot == NULL || p_pPlanRoot->IsNull())
		return;

	if(p_pPlanRoot->Children().empty())
		_triedCases.clear();

	Q.push(p_pPlanRoot);

	while(!Q.empty())
	{
		pCurrentNode = Q.front();
		Q.pop();

		pCurrentPlanStep = pCurrentNode->PlanStep();

		if (pCurrentNode->Type() == PTNTYPE_Goal)
		{
			if (pCurrentNode->IsOpen())
			{
				CaseEx* caseEx = _caseBasedReasoner->Retriever()->Retrieve(pCurrentNode->GetGoal(), g_Game->Self()->State());

				DestroyGoalPlanIfExist(pCurrentNode);

				if (caseEx != NULL &&
					!IsCaseAsTried(pCurrentNode, caseEx))
				{
					MarkCaseAsTried(pCurrentNode, caseEx);
					ExpandGoal(pCurrentNode, caseEx->GetPlanGraph());
					pCurrentNode->Close();

					NotifyChildrenForParentSuccess(pCurrentNode);
				}
				else
				{
					// The planner exhausted all possible plans form the case-base and nothing can succeed
					// We surrender!!
					if (pCurrentNode->SubPlanGoal()->IsNull())
					{
						_planRoot = NULL;
						return;
					}
					else
						pCurrentNode->SubPlanGoal()->Open();
				}
			}
			else
			{
				if (pCurrentPlanStep->State() == ESTATE_Failed)
				{
					pCurrentNode->Open();
				}
				else
				{
					assert(pCurrentPlanStep->State() == ESTATE_Pending ||
						pCurrentPlanStep->State() == ESTATE_Succeeded ||
						pCurrentPlanStep->State() == ESTATE_END);

					pCurrentPlanStep->Update(p_cycles);
					ConsiderReadyChildrenForUpdate(pCurrentNode, Q);
				}
			}
		}
		else if (pCurrentNode->Type() == PTNTYPE_Action)
		{
			assert(pCurrentNode->IsReady());

			if (pCurrentNode->IsOpen())
			{
				if (pCurrentPlanStep->State() == ESTATE_Failed)
				{
					pCurrentNode->SubPlanGoal()->Open();
				}
				else if (pCurrentPlanStep->State() == ESTATE_Succeeded)
				{
					pCurrentNode->Close();
					NotifyChildrenForParentSuccess(pCurrentNode);
				}
				else
				{
					assert(pCurrentPlanStep->State() == ESTATE_NotPrepared ||
						pCurrentPlanStep->State() == ESTATE_Pending ||
						pCurrentPlanStep->State() == ESTATE_Executing ||
						pCurrentPlanStep->State() == ESTATE_END);

					pCurrentPlanStep->Update(p_cycles);
				}
			}
			else
			{
				assert(pCurrentPlanStep->State() == ESTATE_Succeeded);

				ConsiderReadyChildrenForUpdate(pCurrentNode, Q);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::NotifyChildrenForParentSuccess(PlanTreeNodeEx* p_pNode)
{
	for(unsigned i = 0; i < p_pNode->Children().size(); i++)
		p_pNode->Children().at(i)->NotifyParentSuccess(p_pNode);
}
//////////////////////////////////////////////////////////////////////////
void OLCBP::OnlinePlanExpansionExecutionEx::MarkCaseAsTried(PlanTreeNodeEx* p_pStep, CaseEx* p_pCase)
{
	assert(_triedCases[p_pStep].find(p_pCase) == _triedCases[p_pStep].end());
	_triedCases[p_pStep].insert(p_pCase);
}
//////////////////////////////////////////////////////////////////////////
bool OLCBP::OnlinePlanExpansionExecutionEx::IsCaseAsTried(PlanTreeNodeEx* p_pStep, CaseEx* p_pCase)
{
	return _triedCases[p_pStep].find(p_pCase) != _triedCases[p_pStep].end();
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::NotifyMessegeSent(Message* p_message)
{
	queue<PlanTreeNodeEx*>	Q;
	PlanTreeNodeEx*			pCurrentPlanStep;
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
void OnlinePlanExpansionExecutionEx::DestroyGoalPlanIfExist(PlanTreeNodeEx* p_pPlanGoalNode)
{
	queue<PlanTreeNodeEx*>		Q;
	PlanTreeNodeEx*				pCurrentNode;
	PlanTreeNodeEx::NodeList	children(p_pPlanGoalNode->Children());
	PlanTreeNodeEx::NodeList	currentNodeChildren;
	PlanTreeNodeEx::NodeList	preExpansionChildren(p_pPlanGoalNode->BelongingSubPlanChildren());

	// Unlink sub plan roots
	for (PlanTreeNodeEx::NodeList::const_iterator itr = children.begin();
		itr != children.end(); ++itr)
	{
		Q.push(*itr);
		p_pPlanGoalNode->CrossUnlinkChild(*itr);
	}

	// Unlink the rest of the sub plan nodes
	while (!Q.empty())
	{
		pCurrentNode = Q.front();
		Q.pop();

		if (!pCurrentNode->Children().empty())
		{
			currentNodeChildren = pCurrentNode->Children();

			for (PlanTreeNodeEx::NodeList::const_iterator itr = currentNodeChildren.begin();
				itr != currentNodeChildren.end(); ++itr)
			{
				// Nodes from the original sub plan (as appeared in the case plan graph) should not be considered from unlinking from their children
				if ((*itr)->SubPlanGoal() == p_pPlanGoalNode)
				{
					Q.push(*itr);
				}

				pCurrentNode->CrossUnlinkChild(*itr);
			}
		}
		// Consider the nodes memory management
		// delete pCurrentNode;
	}

	assert(p_pPlanGoalNode->Children().empty());

	// Cross link the old pre-expansion children with the plan goal node
	for (PlanTreeNodeEx::NodeList::const_iterator itr = preExpansionChildren.begin();
		itr != preExpansionChildren.end(); ++itr)
	{
		p_pPlanGoalNode->CrossLinkChild(*itr);
	}
}
//////////////////////////////////////////////////////////////////////////
void OnlinePlanExpansionExecutionEx::ConsiderReadyChildrenForUpdate(PlanTreeNodeEx* p_pNode, queue<PlanTreeNodeEx*> &p_updateQueue)
{
	const PlanTreeNodeEx::NodeList& children =  p_pNode->Children();

	for(unsigned i = 0; i < children.size(); i++)
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