#ifndef PLANTREEEX_H
#include "PlanTreeEx.h"
#endif
#ifndef PLANTREENODEEX_H
#include "PlanTreeNodeEx.h"
#endif
#ifndef GOALEX_H
#include "GoalEx.h"
#endif
#ifndef ACTIONEX_H
#include "ActionEx.h"
#endif

PlanTreeEx::PlanTreeEx(PlanTreeNodeEx* p_root)
{
	_root = p_root;
}
//----------------------------------------------------------------------------------------------
void PlanTreeEx::GetReadyOpenGoalsAndReadyActions(vector<PlanTreeNodeEx*> &p_readyOpenGoals, vector<PlanTreeNodeEx*> &p_readyActions)
{
	GetReadyOpenGoalsAndReadyActions(_root, p_readyOpenGoals, p_readyActions);
}
//----------------------------------------------------------------------------------------------
void PlanTreeEx::GetReadyOpenGoalsAndReadyActions(PlanTreeNodeEx* p_planStep, vector<PlanTreeNodeEx*> &p_readyOpenGoals, vector<PlanTreeNodeEx*> &p_readyActions)
{
	if(p_planStep->IsGoal())
	{
		if(p_planStep->GetGoal()->State() == GoalStateType::GSTATE_OpenReady)
		{
			p_readyOpenGoals.push_back(p_planStep);
			return;
		}
		else if(p_planStep->GetGoal()->State() == GoalStateType::GSTATE_OpenWaiting)
		{
			return;
		}
	}
	else
	{
		if(p_planStep->GetAction()->State() == ExecutionStateType::ESTATE_Pending)
		{
			p_readyActions.push_back(p_planStep);
			return;
		}
	}

	vector<PlanTreeNodeEx*> m_children = p_planStep->Children();
	for(int i = 0; i < m_children.size(); i++)
		GetReadyOpenGoalsAndReadyActions(m_children[i], p_readyOpenGoals, p_readyActions);
	return;
}
