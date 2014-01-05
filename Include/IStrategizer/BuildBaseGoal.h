///> [Serializable]
#ifndef BUILDBASEGOAL_H
#define BUILDBASEGOAL_H

#include "GoalEx.h"
#include "EntityClassExist.h"

namespace IStrategizer
{
    ///> class=BuildBaseGoal
    ///> parent=GoalEx
    class BuildBaseGoal : public GoalEx
    {
        OBJECT_SERIALIZABLE(BuildBaseGoal);

    public:
        BuildBaseGoal();
        BuildBaseGoal(const PlanStepParameters& p_parameters);
        
    protected:
        void InitializePostConditions();
        bool SuccessConditionsSatisfied();
    };
}
#endif // BUILDBASEGOAL_H
