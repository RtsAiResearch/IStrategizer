#include "BuildInfrastructureGoal.h"
#include "EntityClassExist.h"
#include "GoalFactory.h"
#include "And.h"
#include "Message.h"
#include "GameEntity.h"
#include "GamePlayer.h"
#include "GameType.h"
#include "RtsGame.h"
#include "PlayerResources.h"
#include "DataMessage.h"

using namespace IStrategizer;
using namespace std;

BuildInfrastructureGoal::BuildInfrastructureGoal() : GoalEx(GOALEX_BuildInfrastructure)
{
    _params[PARAM_EntityClassId] = ECLASS_START;
    _params[PARAM_Amount] = DONT_CARE;
    m_existingAmount = 0;
}
//----------------------------------------------------------------------------------------------
BuildInfrastructureGoal::BuildInfrastructureGoal(const PlanStepParameters& p_parameters): GoalEx(GOALEX_BuildInfrastructure, p_parameters)
{
}
//----------------------------------------------------------------------------------------------
vector<GoalEx*> BuildInfrastructureGoal::GetSucceededInstances(RtsGame &game)
{
    vector<GoalEx*> succeededInstaces(m_succeededInstaces);
    m_succeededInstaces.clear();

    return succeededInstaces;
}
//----------------------------------------------------------------------------------------------
void BuildInfrastructureGoal::HandleMessage(RtsGame& game, Message* p_msg, bool& p_consumed)
{
    if (p_msg->MessageTypeID() == MSG_EntityCreate)
    {
        EntityCreateMessage* pMsg = static_cast<EntityCreateMessage*>(p_msg);
        _ASSERTE(pMsg && pMsg->Data());

        if (pMsg->Data()->OwnerId != PLAYER_Self)
            return;

        EntityClassType entityType = pMsg->Data()->EntityType;
        if (game.GetEntityType(entityType)->Attr(ECATTR_IsBuilding))
        {
            PlanStepParameters params;

            m_createdBuildings[entityType]++;
            params[PARAM_EntityClassId] = entityType;
            params[PARAM_Amount] = m_createdBuildings[entityType];
            m_succeededInstaces.push_back(g_GoalFactory.GetGoal(GOALEX_BuildInfrastructure, params, true));
        }
    }
}
//----------------------------------------------------------------------------------------------
void BuildInfrastructureGoal::InitializePostConditions()
{
    vector<Expression*> m_terms;
    m_terms.push_back(new EntityClassExist(PLAYER_Self, (EntityClassType)_params[PARAM_EntityClassId], _params[PARAM_Amount]));
    _postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
bool BuildInfrastructureGoal::SuccessConditionsSatisfied(RtsGame& game)
{
    vector<TID> entities;
    EntityClassType entityClassType = (EntityClassType)_params[PARAM_EntityClassId];
    int count = 0;

    if (game.Self()->Race()->GetResourceSource(RESOURCE_Supply) == entityClassType)
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

    if (game.Self()->Race()->GetResourceSource(RESOURCE_Supply) == entityClassType)
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
    int availableSupplyBuildings = game.Self()->Resources()->AvailableSupply() / game.Self()->Race()->SupplyBuildingSupplyAmount();
    return availableSupplyBuildings;
}