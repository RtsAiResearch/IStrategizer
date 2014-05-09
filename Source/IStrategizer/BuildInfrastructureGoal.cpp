#include "BuildInfrastructureGoal.h"
#include "EntityClassExist.h"
#include "And.h"
#include "Message.h"
#include "GameEntity.h"
#include "GameEntity.h"
#include "GamePlayer.h"
#include "GameType.h"

using namespace IStrategizer;
using namespace std;

BuildInfrastructureGoal::BuildInfrastructureGoal() : GoalEx(GOALEX_BuildInfrastructure)
{
    _params[PARAM_EntityClassId] = ECLASS_START;
    _params[PARAM_Amount] = 0;
    m_existingAmount = 0;
    m_firstLearningInstance = true;
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

    // This block learns building infra for all buildings with amount 0
    /*if (m_firstLearningInstance)
    {
        m_firstLearningInstance = false;
        
        for(unsigned i = START(EntityClassType); i < END(EntityClassType); ++i)
        {
            EntityClassType entityType = (EntityClassType)i;
            GameType* gameType = game.GetEntityType(entityType);

            if (gameType->Attr(ECATTR_IsBuilding))
            {
                params[PARAM_EntityClassId] = entityType;
                params[PARAM_Amount] = 0;
                succeededInstaces.push_back(new BuildInfrastructureGoal(params));
            }
        }
    }*/

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
    game.Self()->Entities(entityClassType, entities);
    int count = 0;

    for (auto building : entities)
    {
        if (game.Self()->GetEntity(building)->Attr(EOATTR_State) != OBJSTATE_BeingConstructed)
        {
            count++;
        }
    }

    return count >= _params[PARAM_Amount] + m_existingAmount;
}
//----------------------------------------------------------------------------------------------
void BuildInfrastructureGoal::AdaptParameters(RtsGame& game)
{
    vector<TID> entities;
    EntityClassType entityClassType = (EntityClassType)_params[PARAM_EntityClassId];
    game.Self()->Entities(entityClassType, entities);
    m_existingAmount = entities.size();
    _params[PARAM_Amount] = _params[PARAM_Amount] - m_existingAmount;
}