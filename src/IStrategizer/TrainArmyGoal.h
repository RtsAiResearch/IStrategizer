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
        std::vector<GoalEx*> GetSucceededInstances(RtsGame &game);
        bool Equals(PlanStepEx* p_planStep);
        bool Merge(PlanStepEx* planStep);
        unsigned Hash() const { return StepTypeId() + Parameter(PARAM_EntityClassId) + Parameter(PARAM_Amount); }

    protected:
        void InitializePostConditions();
        bool SuccessConditionsSatisfied(RtsGame& game);

    private:
        int m_maxHP;
        int m_maxDamage;
    };
}
#endif // TRAINARMYGOAL_H
