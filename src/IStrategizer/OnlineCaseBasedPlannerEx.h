#ifndef ONLINECASEBASEDPLANNEREX_H
#define ONLINECASEBASEDPLANNEREX_H

#include <string>
#include "EngineData.h"

namespace IStrategizer
{
    enum PlayerType;
    class GoalEx;
    class RtsGame;
    class OnlinePlanExpansionExecution;
    class CaseBasedReasonerEx;

    class OnlineCaseBasedPlannerEx
    {
    private:
        CaseBasedReasonerEx* _caseBasedReasoner;
        OnlinePlanExpansionExecution* _onlineExpansionExecution;

    public:
        OnlineCaseBasedPlannerEx();
        void Update(_In_ RtsGame& game);
        bool Init();
        const CaseBasedReasonerEx* Reasoner() const { return _caseBasedReasoner; }
        const OnlinePlanExpansionExecution* ExpansionExecution() const { return _onlineExpansionExecution; }
        OnlinePlanExpansionExecution* ExpansionExecution() { return _onlineExpansionExecution; }
        virtual ~OnlineCaseBasedPlannerEx();
    };
}

extern IStrategizer::OnlineCaseBasedPlannerEx *g_OnlineCaseBasedPlanner;

#endif    // OnlineCaseBasedPlannerEx