#include "CollectResourceGoal.h"
#include "And.h"
#include "ResourceExist.h"
#include "GameEntity.h"
#include "DataMessage.h"
#include "GameEntity.h"
#include "GamePlayer.h"
#include "GameType.h"

using namespace IStrategizer;

CollectResourceGoal::CollectResourceGoal() : GoalEx(GOALEX_CollectResource)
{
    _params[PARAM_ResourceId] = RESOURCE_START;
    _params[PARAM_ForceSizeId] = FORCESIZE_START;
    m_workersCount = 0;
}
//----------------------------------------------------------------------------------------------
CollectResourceGoal::CollectResourceGoal(const PlanStepParameters& p_parameters): GoalEx(GOALEX_CollectResource, p_parameters)
{
}
//----------------------------------------------------------------------------------------------
void CollectResourceGoal::InitializePostConditions()
{
    vector<Expression*> m_terms;
    m_terms.push_back(new ResourceExist(PLAYER_Self, (ResourceType)_params[PARAM_ResourceId], DONT_CARE));
    _postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
bool CollectResourceGoal::SuccessConditionsSatisfied(RtsGame& game)
{
    vector<TID> entities;
    game.Self()->Entities(game.Self()->GetWorkerType(), entities);
    int gatherersCount = GetNumberOfGatherers(game, (ResourceType)_params[PARAM_ResourceId]);
    int minGatherersCount = game.GetForceSizeCount((ForceSizeType)_params[PARAM_ForceSizeId]);

    return gatherersCount >= minGatherersCount;
}
//----------------------------------------------------------------------------------------------
int CollectResourceGoal::GetNumberOfGatherers(RtsGame &game, ResourceType resourceType) const
{
    int count = 0;
    vector<TID> workers;
    game.Self()->Entities(game.Self()->GetWorkerType(), workers);

    for (auto workerId : workers)
    {
        GameEntity* worker = game.Self()->GetEntity(workerId);
        if (worker->Attr(EOATTR_State) == OBJSTATE_Gathering)
        {
            if (resourceType == RESOURCE_Primary && worker->IsGatheringResource(resourceType))
            {
                ++count;
            }
        }
    }

    return count;
}
//----------------------------------------------------------------------------------------------
void CollectResourceGoal::AddSucceededInstancesForResourceType(RtsGame &game, ResourceType resourceType, vector<GoalEx*>& succeededInstances)
{
    vector<TID> workers;
    game.Self()->Entities(game.Self()->GetWorkerType(), workers);

    if (workers.size() != m_workersCount)
    {
        m_workersCount = workers.size();
        int gatherersCount = GetNumberOfGatherers(game, resourceType);
        ForceSizeType gatherersForceSize = game.GetForceSizeType(gatherersCount);

        PlanStepParameters params;
        params[PARAM_ResourceId] = resourceType;
        params[PARAM_ForceSizeId] = gatherersForceSize;

        succeededInstances.push_back(new CollectResourceGoal(params));
    }
}
//----------------------------------------------------------------------------------------------
vector<GoalEx*> CollectResourceGoal::GetSucceededInstances(RtsGame &game)
{
    vector<GoalEx*> succeededInstances;

    AddSucceededInstancesForResourceType(game, RESOURCE_Primary, succeededInstances);
    AddSucceededInstancesForResourceType(game, RESOURCE_Secondary, succeededInstances);

    return succeededInstances;
}