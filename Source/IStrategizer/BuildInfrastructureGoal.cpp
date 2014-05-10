#include "BuildInfrastructureGoal.h"
#include "EntityClassExist.h"
#include "And.h"
#include "Message.h"
#include "GameEntity.h"
#include "GameEntity.h"
#include "GamePlayer.h"
#include "GameType.h"
#include "RtsGame.h"
#include "PlayerResources.h"

using namespace IStrategizer;
using namespace std;

BuildInfrastructureGoal::BuildInfrastructureGoal() : GoalEx(GOALEX_BuildInfrastructure)
{
    _params[PARAM_EntityClassId] = ECLASS_START;
    _params[PARAM_Amount] = 0;
    m_existingAmount = 0;
}
//----------------------------------------------------------------------------------------------
BuildInfrastructureGoal::BuildInfrastructureGoal(const PlanStepParameters& p_parameters): GoalEx(GOALEX_BuildInfrastructure, p_parameters)
{
}
//----------------------------------------------------------------------------------------------
vector<GoalEx*> BuildInfrastructureGoal::GetSucceededInstances(RtsGame &game)
{
    vector<GoalEx*> succeededInstaces;
    PlanStepParameters params;
    vector<TID> entities;
    map<EntityClassType, int> buildingsCount;
    game.Self()->Entities(entities);

    for (auto entity : entities)
    {
        EntityClassType entityType = game.Self()->GetEntity(entity)->Type();
        GameType* gameType = game.GetEntityType(entityType);

        if (gameType->Attr(ECATTR_IsBuilding))
        {
            buildingsCount[entityType]++;

            if (buildingsCount[entityType] == 1)
            {
                // Special case, learn for goals with zero amount.
                params[PARAM_EntityClassId] = entityType;
                params[PARAM_Amount] = 0;
                succeededInstaces.push_back(new BuildInfrastructureGoal(params));
            }

            params[PARAM_EntityClassId] = entityType;
            params[PARAM_Amount] = buildingsCount[entityType];
            succeededInstaces.push_back(new BuildInfrastructureGoal(params));
        }
    }

    return succeededInstaces;
}
//----------------------------------------------------------------------------------------------
void BuildInfrastructureGoal::InitializePostConditions()
{
    vector<Expression*> m_terms;
    m_terms.push_back(new EntityClassExist(PLAYER_Self, (EntityClassType)_params[PARAM_EntityClassId], DONT_CARE, false));
    _postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
bool BuildInfrastructureGoal::SuccessConditionsSatisfied(RtsGame& game)
{
    vector<TID> entities;
    EntityClassType entityClassType = (EntityClassType)_params[PARAM_EntityClassId];
    int count = 0;

    if (game.GetResourceSource(RESOURCE_Supply) == entityClassType)
    {
        count = GetAvailableSupplyBuildingsCount(game);
    }
    else
    {
        game.Self()->Entities(entityClassType, entities);
        for (auto building : entities)
        {
            if (game.Self()->GetEntity(building)->Attr(EOATTR_State) != OBJSTATE_BeingConstructed)
            {
                count++;
            }
        }
    }

    return count >= _params[PARAM_Amount] + m_existingAmount;
}
//----------------------------------------------------------------------------------------------
void BuildInfrastructureGoal::AdaptParameters(RtsGame& game)
{
    vector<TID> entities;
    EntityClassType entityClassType = (EntityClassType)_params[PARAM_EntityClassId];

    if (game.GetResourceSource(RESOURCE_Supply) == entityClassType)
    {
        m_existingAmount = GetAvailableSupplyBuildingsCount(game);
    }
    else
    {
        game.Self()->Entities(entityClassType, entities);
        m_existingAmount = entities.size();
    }

    _params[PARAM_Amount] = _params[PARAM_Amount] - m_existingAmount;
}
//----------------------------------------------------------------------------------------------
int BuildInfrastructureGoal::GetAvailableSupplyBuildingsCount(RtsGame &game) const
{
    // The availableSupplyBuildings won't match the exact number of supply buildings in the game
    int availableSupplyBuildings = game.Self()->Resources()->AvailableSupply() / game.SupplyBuildingSupplyAmount();
    return availableSupplyBuildings;
}