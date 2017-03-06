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
#include "ObjectFactory.h"

using namespace IStrategizer;
using namespace std;

DECL_SERIALIZABLE(BuildInfrastructureGoal);

BuildInfrastructureGoal::BuildInfrastructureGoal() : GoalEx(GOALEX_BuildInfrastructure)
{
    _params[PARAM_EntityClassId] = ECLASS_START;
    _params[PARAM_Amount] = DONT_CARE;
    m_existingAmount = 0;
}
//----------------------------------------------------------------------------------------------
BuildInfrastructureGoal::BuildInfrastructureGoal(const PlanStepParameters& p_parameters) : GoalEx(GOALEX_BuildInfrastructure, p_parameters)
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
    if (p_msg->TypeId() == MSG_EntityCreate || p_msg->TypeId() == MSG_EntityRenegade)
    {
        EntityCreateMessage* pMsg = static_cast<EntityCreateMessage*>(p_msg);
        _ASSERTE(pMsg && pMsg->Data());

        if (pMsg->Data()->OwnerId != PLAYER_Self)
            return;

        EntityClassType entityType = pMsg->Data()->EntityType;
        if (game.GetEntityType(entityType)->P(TP_IsBuilding))
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
    EntityClassType entityClassType = (EntityClassType)_params[PARAM_EntityClassId];
    int count = 0;

    for (auto& entityR : game.Self()->Entities())
    {
        if (entityR.second->TypeId() == entityClassType &&
            entityR.second->P(OP_State) != OBJSTATE_BeingConstructed)
        {
            count++;
        }
    }

    return count >= _params[PARAM_Amount] + m_existingAmount;
}
//----------------------------------------------------------------------------------------------
void BuildInfrastructureGoal::AdaptParameters(RtsGame& game)
{
    EntityList entities;
    EntityClassType entityClassType = (EntityClassType)_params[PARAM_EntityClassId];

    /* if (game.Self()->Race()->GetResourceSource(RESOURCE_Supply) == entityClassType)
     {
     m_existingAmount = GetAvailableSupplyBuildingsCount(game);
     }
     else
     {*/
    game.Self()->Entities(entityClassType, entities);
    m_existingAmount = entities.size();
    //}

    _params[PARAM_Amount] = _params[PARAM_Amount] - m_existingAmount;
}
//----------------------------------------------------------------------------------------------
int BuildInfrastructureGoal::GetAvailableSupplyBuildingsCount(RtsGame &game) const
{
    // The availableSupplyBuildings won't match the exact number of supply buildings in the game
    int availableSupplyBuildings = game.Self()->Resources()->AvailableSupply() / game.Self()->Race()->SupplyBuildingSupplyAmount();
    return availableSupplyBuildings;
}
//----------------------------------------------------------------------------------------------
bool BuildInfrastructureGoal::Equals(PlanStepEx* p_planStep)
{
    return StepTypeId() == p_planStep->StepTypeId() &&
        _params[PARAM_EntityClassId] == p_planStep->Parameter(PARAM_EntityClassId) &&
        _params[PARAM_Amount] == p_planStep->Parameter(PARAM_Amount);
}
//----------------------------------------------------------------------------------------------
bool BuildInfrastructureGoal::Merge(PlanStepEx* planStep)
{
    if (StepTypeId() == planStep->StepTypeId())
    {
        if (_params[PARAM_EntityClassId] == planStep->Parameter(PARAM_EntityClassId))
        {
            _params[PARAM_Amount] += planStep->Parameter(PARAM_Amount);
            return true;
        }
    }

    return false;
}