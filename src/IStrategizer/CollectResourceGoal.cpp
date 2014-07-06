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
using namespace std;

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
    int gatherersCount = GetNumberOfGatherers(game, (ResourceType)_params[PARAM_ResourceId]);
    int minGatherersCount = _params[PARAM_Amount];

    return gatherersCount >= minGatherersCount;
}
//----------------------------------------------------------------------------------------------
int CollectResourceGoal::GetNumberOfGatherers(RtsGame &game, ResourceType resourceType)
{
    EntityList workers;
    game.Self()->Entities(game.Self()->Race()->GetWorkerType(), workers);

    for (auto workerId : workers)
    {
        GameEntity* worker = game.Self()->GetEntity(workerId);
        if ((resourceType == RESOURCE_Primary && worker->Attr(EOATTR_State) == OBJSTATE_GatheringPrimary) || 
            (resourceType == RESOURCE_Secondary && worker->Attr(EOATTR_State) == OBJSTATE_GatheringSecondary))
        {
            m_observedGatherers[resourceType].insert(workerId);
        }
    }

    return m_observedGatherers[resourceType].size();
}
//----------------------------------------------------------------------------------------------
void CollectResourceGoal::AddSucceededInstancesForResourceType(RtsGame &game, ResourceType resourceType, vector<GoalEx*>& succeededInstances)
{
    EntityList workers;
    game.Self()->Entities(game.Self()->Race()->GetWorkerType(), workers);

    int gatherersCount = GetNumberOfGatherers(game, resourceType);

    if (gatherersCount > 0)
    {
        if (resourceType == RESOURCE_Primary && m_succeededPrimaryGatherersCount.count(gatherersCount) == 0)
        {
            PlanStepParameters params;
            params[PARAM_ResourceId] = resourceType;
            params[PARAM_Amount] = gatherersCount;
            succeededInstances.push_back(g_GoalFactory.GetGoal(GOALEX_CollectResource, params, true));

            m_succeededPrimaryGatherersCount.insert(gatherersCount);
        }

        if (resourceType == RESOURCE_Secondary && m_succeededSecondaryGatherersCount.count(gatherersCount) == 0)
        {
            PlanStepParameters params;
            params[PARAM_ResourceId] = resourceType;
            params[PARAM_Amount] = gatherersCount;
            succeededInstances.push_back(g_GoalFactory.GetGoal(GOALEX_CollectResource, params, true));

            m_succeededSecondaryGatherersCount.insert(gatherersCount);
        }
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
//----------------------------------------------------------------------------------------------
bool CollectResourceGoal::Equals(PlanStepEx* p_planStep)
{
    return StepTypeId() == p_planStep->StepTypeId() &&
        _params[PARAM_ResourceId] == p_planStep->Parameter(PARAM_ResourceId) &&
        _params[PARAM_Amount] == p_planStep->Parameter(PARAM_Amount);
}
//----------------------------------------------------------------------------------------------
bool CollectResourceGoal::Merge(PlanStepEx* planStep)
{
    if (StepTypeId() == planStep->StepTypeId())
    {
        if (_params[PARAM_ResourceId] == planStep->Parameter(PARAM_ResourceId))
        {
            _params[PARAM_Amount] += planStep->Parameter(PARAM_Amount);
            return true;
        }
    }

    return false;
}