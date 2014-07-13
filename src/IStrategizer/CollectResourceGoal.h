///> [Serializable]
#ifndef COLLECTRESOURCEGOAL_H
#define COLLECTRESOURCEGOAL_H

#include "GoalEx.h"
#include "EntityClassExist.h"
#include <vector>
#include <set>

namespace IStrategizer
{
    ///> class=CollectResourceGoal
    ///> parent=GoalEx
    class CollectResourceGoal : public GoalEx
    {
        OBJECT_SERIALIZABLE_P(CollectResourceGoal, GoalEx);

    public:
        CollectResourceGoal();
        CollectResourceGoal(const PlanStepParameters& p_parameters);
        std::vector<GoalEx*> GetSucceededInstances(RtsGame &game);
        bool Equals(PlanStepEx* p_planStep);
        bool Merge(PlanStepEx* planStep);
        
    protected:
        void InitializePostConditions();
        bool SuccessConditionsSatisfied(RtsGame& game);

    private:
        std::set<int> m_succeededPrimaryGatherersCount;
        std::set<int> m_succeededSecondaryGatherersCount;

        int GetNumberOfGatherers(RtsGame &game, ResourceType resourceType) ;
        void AddSucceededInstancesForResourceType(RtsGame &game, ResourceType resourceType, std::vector<GoalEx*>& succeededInstances);

        std::map<ResourceType, std::set<TID>> m_observedGatherers;
    };
}
#endif // COLLECTRESOURCEGOAL_H
