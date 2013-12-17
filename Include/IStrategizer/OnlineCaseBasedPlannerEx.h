#ifndef ONLINECASEBASEDPLANNEREX_H
#define ONLINECASEBASEDPLANNEREX_H

#include <string>
class GoalEx;

namespace IStrategizer
{
	enum PlayerType;
}

using namespace IStrategizer;
namespace IStrategizer
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
		const CaseBasedReasonerEx* Reasoner() const { return _caseBasedReasoner; }
		const OnlinePlanExpansionExecutionEx* ExpansionExecution() const { return _onlineExpansionExecution; }
        virtual    ~OnlineCaseBasedPlannerEx();
    };
}

extern IStrategizer::OnlineCaseBasedPlannerEx *g_OnlineCaseBasedPlanner;

#endif	// OnlineCaseBasedPlannerEx