#ifndef ONLINECASEBASEDPLANNEREX_H
#define ONLINECASEBASEDPLANNEREX_H

#include <string>
#include "WorldClock.h"

namespace IStrategizer
{
	enum PlayerType;
	class GoalEx;

	const std::string g_CaseBasePath = "IStrategizerEx.cb";

	class OnlinePlanExpansionExecutionEx;
	class CaseBasedReasonerEx;

	class OnlineCaseBasedPlannerEx
	{
	private:
		CaseBasedReasonerEx*			_caseBasedReasoner;
		OnlinePlanExpansionExecutionEx*	_onlineExpansionExecution;

	public:
		OnlineCaseBasedPlannerEx();
		void       Update(const WorldClock& p_clock);
		void       Init(GoalEx *p_initialGoal);
		const CaseBasedReasonerEx* Reasoner() const { return _caseBasedReasoner; }
		const OnlinePlanExpansionExecutionEx* ExpansionExecution() const { return _onlineExpansionExecution; }
		virtual    ~OnlineCaseBasedPlannerEx();
	};
}

extern IStrategizer::OnlineCaseBasedPlannerEx *g_OnlineCaseBasedPlanner;

#endif	// OnlineCaseBasedPlannerEx