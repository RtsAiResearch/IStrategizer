#ifndef ONLINEPLANEXPANSIONEXECUTIONEX_H
#define ONLINEPLANEXPANSIONEXECUTIONEX_H

#define MAX_GOAL_WAITING_TIME 10000ll
#define MAX_ACTION_WAITING_TIME 10000ll

#include <map>
#include <vector>
#include <set>
#include <queue>

using namespace std;

#ifndef MESSAGEPUMPOBSERVER_H
#include "MessagePumpObserver.h"
#endif

class PlanGraph;
class CaseEx;
class GoalEx;

namespace MetaData
{
	enum PlayerType;
}

using namespace MetaData;
namespace OLCBP
{
	class PlanTreeNodeEx;
	class CaseBasedReasonerEx;
	class OnlinePlanExpansionExecutionEx : public MessagePumpObserver
	{

	public:
		OnlinePlanExpansionExecutionEx(GoalEx* p_initialGoal, CaseBasedReasonerEx* p_casedBasedReasoner);
		void Update(unsigned long p_cycles);
		void NotifyMessegeSent(Message* p_message);

	private:
		void ExpandGoal(PlanTreeNodeEx* p_rootGoal, PlanGraph* p_supPlan);
		void UpdatePlan(PlanTreeNodeEx* p_rootPlanStep, unsigned long p_cycles);
		void NotifyChildrenForParentSuccess(PlanTreeNodeEx* p_pNode);
		void MarkCaseAsTried(PlanTreeNodeEx* p_pStep, CaseEx* p_pCase);
		bool IsCaseAsTried(PlanTreeNodeEx* p_pStep, CaseEx* p_pCase);
		void DestroyGoalPlanIfExist(PlanTreeNodeEx* p_pPlanGoalNode);
		void ConsiderReadyChildrenForUpdate(PlanTreeNodeEx* p_pNode, queue<PlanTreeNodeEx*> &p_updateQueue);

		PlanTreeNodeEx*						_planRoot;
		CaseBasedReasonerEx*				_caseBasedReasoner;
		map<PlanTreeNodeEx*, set<CaseEx*> > _triedCases;
		map<PlanTreeNodeEx*, CaseEx*>		_cases;
	};
}


#endif	// ONLINEPLANEXPANSIONEXECUTIONEX_H