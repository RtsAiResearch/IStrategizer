///> [Serializable]
#ifndef COLLECTRESOURCEGOAL_H
#define COLLECTRESOURCEGOAL_H

#include "GoalEx.h"
#include "EntityClassExist.h"

namespace IStrategizer
{
    ///> class=CollectResourceGoal
    ///> parent=GoalEx
    class CollectResourceGoal : public GoalEx
    {
        OBJECT_SERIALIZABLE(CollectResourceGoal);

    public:
        CollectResourceGoal();
        CollectResourceGoal(const PlanStepParameters& p_parameters);
        std::vector<GoalEx*> GetSucceededInstances(RtsGame &game);
        
    protected:
        void InitializePostConditions();
        bool SuccessConditionsSatisfied(RtsGame& game);

    private:
        size_t m_workersCount;

        int GetNumberOfGatherers(RtsGame &game, ResourceType resourceType) const;
        void AddSucceededInstancesForResourceType(RtsGame &game, ResourceType resourceType, vector<GoalEx*>& succeededInstances);
    };
}
#endif // COLLECTRESOURCEGOAL_H
