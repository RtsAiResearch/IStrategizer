#include "CollectResourceGoal.h"
#include "And.h"
#include "ResourceExist.h"
#include "GameEntity.h"
#include "DataMessage.h"
#include "GameEntity.h"
#include "GamePlayer.h"
#include "GameType.h"
#include "GoalFactory.h"

using namespace IStrategizer;

CollectResourceGoal::CollectResourceGoal() : GoalEx(GOALEX_CollectResource)
{
    _params[PARAM_ResourceId] = RESOURCE_START;
    _params[PARAM_Amount] = DONT_CARE;
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
    game.Self()->Entities(game.Self()->Race()->GetWorkerType(), entities);
    int gatherersCount = GetNumberOfGatherers(game, (ResourceType)_params[PARAM_ResourceId]);
    int minGatherersCount = _params[PARAM_Amount];

    return gatherersCount >= minGatherersCount;
}
//----------------------------------------------------------------------------------------------
int CollectResourceGoal::GetNumberOfGatherers(RtsGame &game, ResourceType resourceType) const
{
    int count = 0;
    vector<TID> workers;
    game.Self()->Entities(game.Self()->Race()->GetWorkerType(), workers);

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
    game.Self()->Entities(game.Self()->Race()->GetWorkerType(), workers);

    int gatherersCount = GetNumberOfGatherers(game, resourceType);

    if (gatherersCount != 0 && gatherersCount <= 6 && find(m_succeededGatherersCount.begin(), m_succeededGatherersCount.end(), gatherersCount) == m_succeededGatherersCount.end())
    {
        PlanStepParameters params;
        params[PARAM_ResourceId] = resourceType;
        params[PARAM_Amount] = gatherersCount;
        succeededInstances.push_back(g_GoalFactory.GetGoal(GOALEX_CollectResource, params, true));

        m_succeededGatherersCount.push_back(gatherersCount);
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