#ifndef ONLINEPLANEXPANSIONEXECUTIONEX_H
#define ONLINEPLANEXPANSIONEXECUTIONEX_H

#define MAX_GOAL_WAITING_TIME 10000ll
#define MAX_ACTION_WAITING_TIME 10000ll

#include <map>
#include "PlanTreeNodeEx.h"
using namespace std;

#ifndef ENGINECOMPONENT_H
#include "EngineComponent.h"
#endif

namespace IStrategizer
{
	enum PlayerType;
	class PlanGraph;
	class CaseEx;
	class GoalEx;
	class CaseBasedReasonerEx;

	class OnlinePlanExpansionExecutionEx : public EngineComponent
	{
	public:
		typedef std::set<CaseEx*> CaseSet;

		OnlinePlanExpansionExecutionEx(GoalEx* p_initialGoal, CaseBasedReasonerEx* p_casedBasedReasoner);
		void Update(unsigned long p_cycles);
		void NotifyMessegeSent(Message* p_message);
		const PlanTreeNodeEx* PlanRoot() const { return _planRoot; }

	private:
		void ExpandGoal(PlanTreeNodeEx* p_rootGoal, CaseEx* p_pCase);
		void UpdatePlan(PlanTreeNodeEx* p_rootPlanStep, unsigned long p_cycles);


		void NotifyChildrenForParentSuccess(PlanTreeNodeEx* p_pNode);
		void MarkCaseAsTried(PlanTreeNodeEx* p_pStep, CaseEx* p_pCase);
		bool IsCaseTried(PlanTreeNodeEx* p_pStep, CaseEx* p_pCase);
		bool DestroyGoalPlanIfExist(PlanTreeNodeEx* p_pPlanGoalNode);
		void ConsiderReadyChildrenForUpdate(PlanTreeNodeEx* p_pNode, PlanTreeNodeEx::Queue &p_updateQueue);
		void UpdateActionNode(PlanTreeNodeEx* pCurrentNode, unsigned long p_cycles, PlanTreeNodeEx::Queue& p_updateQ);
		void UpdateGoalNode(PlanTreeNodeEx* p_pCurrentNode, unsigned p_cycles, PlanTreeNodeEx::Queue& p_updateQ);

		PlanTreeNodeEx*						_planRoot;
		CaseBasedReasonerEx*				_caseBasedReasoner;
		std::map<PlanTreeNodeEx*, CaseSet>	_triedCases;
		std::map<PlanTreeNodeEx*, CaseEx*>	_cases;
	};
}

#endif	// ONLINEPLANEXPANSIONEXECUTIONEX_H