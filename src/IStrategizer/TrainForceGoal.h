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
        std::vector<GoalEx*> GetSucceededInstances(RtsGame &game);
        bool Equals(PlanStepEx* p_planStep);
        bool Merge(PlanStepEx* planStep);

    protected:
        void InitializePostConditions();
        bool SuccessConditionsSatisfied(RtsGame& game);

    private:
        std::map<EntityClassType, int> m_trainedUnits;
        EntitySet m_usedUnits;
        bool m_firstUpdate;
    };
}
#endif // TRAINFORCEGOAL_H
