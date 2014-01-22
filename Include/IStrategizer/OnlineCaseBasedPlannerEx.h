#ifndef ONLINECASEBASEDPLANNEREX_H
#define ONLINECASEBASEDPLANNEREX_H

#include <string>
#include "WorldClock.h"
#include "RtsGame.h"

namespace IStrategizer
{
    enum PlayerType;
    class GoalEx;

    class OnlinePlanExpansionExecutionEx;
    class CaseBasedReasonerEx;

    class OnlineCaseBasedPlannerEx
    {
    private:
        CaseBasedReasonerEx*            _caseBasedReasoner;
        OnlinePlanExpansionExecutionEx*    _onlineExpansionExecution;

    public:
        OnlineCaseBasedPlannerEx();
        void       Update(RtsGame& p_RtsGame, const WorldClock& p_clock);
        void       Init(RtsGame& p_RtsGame, GoalEx *p_initialGoal);
        const CaseBasedReasonerEx* Reasoner() const { return _caseBasedReasoner; }
        const OnlinePlanExpansionExecutionEx* ExpansionExecution() const { return _onlineExpansionExecution; }
        OnlinePlanExpansionExecutionEx* ExpansionExecution() { return _onlineExpansionExecution; }
        virtual    ~OnlineCaseBasedPlannerEx();
    };
}

extern IStrategizer::OnlineCaseBasedPlannerEx *g_OnlineCaseBasedPlanner;

#endif    // OnlineCaseBasedPlannerEx