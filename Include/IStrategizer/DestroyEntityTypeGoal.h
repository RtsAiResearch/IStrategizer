///> [Serializable]
#ifndef DESTROYENTITYTYPEGOAL_H
#define DESTROYENTITYTYPEGOAL_H

#include "GoalEx.h"

namespace IStrategizer
{
    ///> class=DestroyEntityTypeGoal
    ///> parent=GoalEx
    class DestroyEntityTypeGoal : public GoalEx
    {
        OBJECT_SERIALIZABLE(DestroyEntityTypeGoal);

    private:
        ForceDescriptionEx _forceDescription;

    public:
        DestroyEntityTypeGoal();
        DestroyEntityTypeGoal(const PlanStepParameters& p_parameters);
        void Copy(IClonable* p_dest);

    protected:
        void InitializePostConditions();
        bool SuccessConditionsSatisfied(RtsGame& p_RtsGame);
    };
}
#endif // DESTROYENTITYTYPEGOAL_H
