///> [Serializable]
#ifndef WINWARGUSGOALEX_H
#define WINWARGUSGOALEX_H

#include "GoalEx.h"

namespace IStrategizer
{
    ///> class=WinWargusGoalEx
    ///> parent=GoalEx
    class WinWargusGoalEx : public GoalEx
    {
        OBJECT_SERIALIZABLE(WinWargusGoalEx);

    public:
        WinWargusGoalEx();
        WinWargusGoalEx(const PlanStepParameters& p_parameters);
        
    protected:
        void InitializePostConditions();
        bool SuccessConditionsSatisfied(RtsGame* pRtsGame);
    };
}

#endif // WINWARGUSGOALEX_H
