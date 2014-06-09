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
        void HandleMessage(RtsGame& game, Message* p_msg, bool& p_consumed);
        std::vector<GoalEx*> GetSucceededInstances(RtsGame &game);
        bool Equals(PlanStepEx* p_planStep);
        unsigned Hash() const { return StepTypeId() + Parameter(PARAM_EntityClassId) + Parameter(PARAM_Amount); }

    protected:
        void InitializePostConditions();
        bool SuccessConditionsSatisfied(RtsGame& game);

    private:
        std::map<EntityClassType, int> m_trainedUnits;
        std::vector<GoalEx*> m_succeededInstances;
    };
}
#endif // TRAINFORCEGOAL_H
