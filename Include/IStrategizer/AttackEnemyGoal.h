///> [Serializable]
#ifndef ATTACKENEMYGOAL_H
#define ATTACKENEMYGOAL_H

#include "GoalEx.h"
#include "Colony.h"

namespace IStrategizer
{
    ///> class=AttackEnemyGoal
    ///> parent=GoalEx
    class AttackEnemyGoal : public GoalEx
    {
        OBJECT_SERIALIZABLE(AttackEnemyGoal);

    private:
        ForceDescriptionEx _forceDescription;
        Colony             _cachedColony;

    public:
                    AttackEnemyGoal();
                    AttackEnemyGoal(const PlanStepParameters& p_parameters);
        void InitSuccessConditions();
        void        Copy(IClonable* p_dest);

    protected:
        void InitializeSuccessConditions();
        void InitializePostConditions();
    };
}
#endif // ATTACKENEMYGOAL_H
