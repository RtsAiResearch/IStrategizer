///> [Serializable]
#ifndef DEPLOYARMYGOAL_H
#define DEPLOYARMYGOAL_H

#include "GoalEx.h"
#include <vector>

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
        std::vector<GoalEx*> GetSucceededInstances(RtsGame &game);
        bool Equals(PlanStepEx* p_planStep);
        bool Merge(PlanStepEx* planStep) { return false; }
        unsigned Hash() const { return StepTypeId() + Parameter(PARAM_DeployType) + Parameter(PARAM_EntityClassId); }
        
    protected:
        void InitializePostConditions();
        bool SuccessConditionsSatisfied(RtsGame& game);
    };
}

#endif // DEPLOYARMYGOAL_H
