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
        int _demandTargetSize;
    public:
        DestroyEntityTypeGoal();
        DestroyEntityTypeGoal(const PlanStepParameters& p_parameters);
        void Copy(IClonable* p_dest);
        void HandleMessage(RtsGame& pRtsGame, Message* p_msg, bool& p_consumed);

    protected:
        void InitializePostConditions();
        bool SuccessConditionsSatisfied(RtsGame& pRtsGame);
    };
}
#endif // DESTROYENTITYTYPEGOAL_H
