///> [Serializable]
#ifndef DESTROYENTITYTYPEGOAL_H
#define DESTROYENTITYTYPEGOAL_H

#include "GoalEx.h"
#include "Colony.h"

namespace IStrategizer
{
    ///> class=DestroyEntityTypeGoal
    ///> parent=GoalEx
    class DestroyEntityTypeGoal : public GoalEx
    {
        OBJECT_SERIALIZABLE(DestroyEntityTypeGoal);

    private:
        ForceDescriptionEx _forceDescription;
        Colony _cachedColony;

    public:
        DestroyEntityTypeGoal();
        DestroyEntityTypeGoal(const PlanStepParameters& p_parameters);
        void Copy(IClonable* p_dest);

    protected:
        void InitializePostConditions();
        bool SuccessConditionsSatisfied(RtsGame* pRtsGame);
    };
}
#endif // DESTROYENTITYTYPEGOAL_H
