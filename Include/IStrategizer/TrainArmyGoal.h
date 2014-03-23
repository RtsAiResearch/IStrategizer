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
        void HandleMessage(RtsGame& pRtsGame, Message* p_msg, bool& p_consumed);

    protected:
        void InitializePostConditions();
        bool SuccessConditionsSatisfied(RtsGame& pRtsGame);

    private:
        int _demandSize;
    };
}
#endif // TRAINARMYGOAL_H
