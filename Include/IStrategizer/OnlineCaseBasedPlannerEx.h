#ifndef ONLINECASEBASEDPLANNEREX_H
#define ONLINECASEBASEDPLANNEREX_H

#include <string>
class GoalEx;

namespace MetaData
{
	enum PlayerType;
}

using namespace MetaData;
namespace OLCBP
{
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
        void       Update(unsigned long p_gameCycle);
        void       Init(GoalEx *p_initialGoal);
		CaseBasedReasonerEx* Reasoner() const { return _caseBasedReasoner; }
		OnlinePlanExpansionExecutionEx* Planner() const { return _onlineExpansionExecution; }
        virtual    ~OnlineCaseBasedPlannerEx();
    };
}

extern OLCBP::OnlineCaseBasedPlannerEx *g_OnlineCaseBasedPlanner;

#endif	// OnlineCaseBasedPlannerEx