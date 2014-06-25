#ifndef ONLINECASEBASEDPLANNEREX_H
#define ONLINECASEBASEDPLANNEREX_H

#include <string>
#include "WorldClock.h"
#include "EngineData.h"

namespace IStrategizer
{
    enum PlayerType;
    class GoalEx;

    class OnlinePlanExpansionExecution;
    class CaseBasedReasonerEx;

    class OnlineCaseBasedPlannerEx
    {
    private:
        CaseBasedReasonerEx* _caseBasedReasoner;
        OnlinePlanExpansionExecution* _onlineExpansionExecution;

    public:
        OnlineCaseBasedPlannerEx();
        void Update(const WorldClock& p_clock);
        void Init(GoalEx *p_initialGoal);
        void Init(GoalType p_goalType);
        const CaseBasedReasonerEx* Reasoner() const { return _caseBasedReasoner; }
        const OnlinePlanExpansionExecution* ExpansionExecution() const { return _onlineExpansionExecution; }
        OnlinePlanExpansionExecution* ExpansionExecution() { return _onlineExpansionExecution; }
        virtual ~OnlineCaseBasedPlannerEx();
    };
}

extern IStrategizer::OnlineCaseBasedPlannerEx *g_OnlineCaseBasedPlanner;

#endif    // OnlineCaseBasedPlannerEx