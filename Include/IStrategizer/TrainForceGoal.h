///> [Serializable]
#ifndef TRAINFORCEGOAL_H
#define TRAINFORCEGOAL_H

#include "GoalEx.h"

namespace IStrategizer
{
    ///> class=TrainForceGoal
    ///> parent=GoalEx
    class TrainForceGoal : public GoalEx
    {
        OBJECT_SERIALIZABLE(TrainForceGoal);

    public:
        TrainForceGoal();
        TrainForceGoal(const PlanStepParameters& p_parameters);
    
    protected:
        void InitializePostConditions();
        bool SuccessConditionsSatisfied(RtsGame* pRtsGame);
    };
}
#endif // TRAINFORCEGOAL_H
