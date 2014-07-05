#include "TrainForceGoal.h"
#include "EntityClassExist.h"
#include "And.h"
#include "Message.h"
#include "DataMessage.h"
#include "GameEntity.h"
#include "DataMessage.h"
#include "GameEntity.h"
#include "GamePlayer.h"
#include "GameType.h"
#include "GoalFactory.h"

using namespace IStrategizer;
using namespace std;

TrainForceGoal::TrainForceGoal() : GoalEx(GOALEX_TrainForce), m_firstUpdate(true)
{
    _params[PARAM_Amount] = DONT_CARE;
    _params[PARAM_EntityClassId] = ECLASS_START;
    _params[PARAM_ObjectStateType] = OBJSTATE_START;
}
//----------------------------------------------------------------------------------------------
TrainForceGoal::TrainForceGoal(const PlanStepParameters& p_parameters): GoalEx(GOALEX_TrainForce, p_parameters), m_firstUpdate(true)
{
}
//----------------------------------------------------------------------------------------------
void TrainForceGoal::InitializePostConditions()
{
    vector<Expression*> m_terms;
    m_terms.push_back(new EntityClassExist(PLAYER_Self, (EntityClassType)_params[PARAM_EntityClassId], _params[PARAM_Amount]));
    _postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
bool TrainForceGoal::SuccessConditionsSatisfied(RtsGame& game)
{
    EntityList entities;
    game.Self()->Entities((EntityClassType)_params[PARAM_EntityClassId], entities);
    int entitiesCount = 0;
    int requiredCount = _params[PARAM_Amount];

    for (TID unitId : entities)
    {
        GameEntity *pEntity = game.Self()->GetEntity(unitId);
        _ASSERTE(pEntity);
        entitiesCount +=  g_Assist.IsEntityInState(unitId, (ObjectStateType)_params[PARAM_ObjectStateType]);
    }

    return entitiesCount >= requiredCount;
}
//----------------------------------------------------------------------------------------------
vector<GoalEx*> TrainForceGoal::GetSucceededInstances(RtsGame &game)
{
    vector<GoalEx*> succeededInstances;

    EntityList entities;
    game.Self()->Entities(entities);

    for (TID entityId : entities)
    {
        GameEntity *pEntity = game.Self()->GetEntity(entityId);
        _ASSERTE(pEntity);
        EntityClassType entityType = pEntity->Type();

        if (m_usedUnits.count(entityId) == 0 &&
            !game.GetEntityType(entityType)->Attr(ECATTR_IsBuilding) &&
            g_Assist.IsEntityObjectReady(entityId) &&
            ((!game.GetEntityType(entityType)->Attr(ECATTR_IsAttacker)) ||
            (pEntity->Attr(EOATTR_State) != OBJSTATE_Idle)))
        {
            PlanStepParameters params;
            EntityList entities;
            game.Self()->Entities(entityType, entities);
            m_trainedUnits[entityType] = entities.size();
            params[PARAM_EntityClassId] = entityType;
            params[PARAM_Amount] = m_trainedUnits[entityType];
            params[PARAM_ObjectStateType] = pEntity->Attr(EOATTR_State);
            succeededInstances.push_back(g_GoalFactory.GetGoal(GOALEX_TrainForce, params, true));
            m_usedUnits.insert(entityId);
        }
    }

    if (m_firstUpdate)
    {
        m_firstUpdate = false;
        succeededInstances.clear();
    }

    return succeededInstances;
}
//----------------------------------------------------------------------------------------------
bool TrainForceGoal::Equals(PlanStepEx* p_planStep)
{
    return StepTypeId() == p_planStep->StepTypeId() &&
        _params[PARAM_EntityClassId] == p_planStep->Parameter(PARAM_EntityClassId) &&
        _params[PARAM_Amount] == p_planStep->Parameter(PARAM_Amount);
}
//----------------------------------------------------------------------------------------------
bool TrainForceGoal::Merge(PlanStepEx* planStep)
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