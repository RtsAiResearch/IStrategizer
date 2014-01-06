///> [Serializable]
#ifndef DEPLOYARMYGOAL_H
#define DEPLOYARMYGOAL_H

#include "GoalEx.h"

namespace IStrategizer
{
    ///> class=DeployArmyGoal
    ///> parent=GoalEx
    class DeployArmyGoal : public GoalEx
    {
        OBJECT_SERIALIZABLE(DeployArmyGoal);

    public:
        DeployArmyGoal();
        DeployArmyGoal(const PlanStepParameters& p_parameters);
        
    protected:
        void InitializePostConditions();
        bool SuccessConditionsSatisfied(RtsGame* pRtsGame);
    };
}

#endif // DEPLOYARMYGOAL_H
