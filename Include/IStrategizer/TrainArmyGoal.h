///> [Serializable]
#ifndef TRAINARMYGOAL_H
#define TRAINARMYGOAL_H

#include "GoalEx.h"

namespace IStrategizer
{
    ///> class=TrainArmyGoal
    ///> parent=GoalEx
    class TrainArmyGoal : public GoalEx
    {
        OBJECT_SERIALIZABLE(TrainArmyGoal);

    public:
        TrainArmyGoal();
        TrainArmyGoal(const PlanStepParameters& p_parameters);
    
    protected:
        void InitializePostConditions();
        bool SuccessConditionsSatisfied(RtsGame* pRtsGame);
    };
}
#endif // TRAINARMYGOAL_H
