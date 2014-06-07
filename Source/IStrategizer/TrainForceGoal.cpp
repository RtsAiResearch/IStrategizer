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

TrainForceGoal::TrainForceGoal() : GoalEx(GOALEX_TrainForce)
{
    _params[PARAM_Amount] = DONT_CARE;
    _params[PARAM_EntityClassId] = ECLASS_START;
}
//----------------------------------------------------------------------------------------------
TrainForceGoal::TrainForceGoal(const PlanStepParameters& p_parameters): GoalEx(GOALEX_TrainForce, p_parameters)
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
    vector<TID> entities;
    game.Self()->Entities((EntityClassType)_params[PARAM_EntityClassId], entities);
    int entitiesCount = 0;
    int requiredCount = _params[PARAM_Amount];

    for (TID unitId : entities)
    {
        GameEntity *pEntity = game.Self()->GetEntity(unitId);
        _ASSERTE(pEntity);
        entitiesCount += pEntity->Attr(EOATTR_State) != OBJSTATE_BeingConstructed ? 1 : 0;
    }

    return entitiesCount >= requiredCount;
}
//----------------------------------------------------------------------------------------------
void TrainForceGoal::HandleMessage(RtsGame& game, Message* p_msg, bool& p_consumed)
{
    if (p_msg->MessageTypeID() == MSG_EntityCreate)
    {
        EntityCreateMessage* pMsg = static_cast<EntityCreateMessage*>(p_msg);
        _ASSERTE(pMsg && pMsg->Data());

        if (pMsg->Data()->OwnerId != PLAYER_Self)
            return;

        TID entityId = pMsg->Data()->EntityId;
        GameEntity *pEntity = game.Self()->GetEntity(entityId);
        _ASSERTE(pEntity);
        EntityClassType entityType = pEntity->Type();

        if (!game.GetEntityType(entityType)->Attr(ECATTR_IsBuilding))
        {
            PlanStepParameters params;
            vector<TID> entities;
            game.Self()->Entities(entityType, entities);
            m_trainedUnits[entityType] = entities.size();
            params[PARAM_EntityClassId] = entityType;
            params[PARAM_Amount] = m_trainedUnits[entityType];
            m_succeededInstances.push_back(g_GoalFactory.GetGoal(GOALEX_TrainForce, params, true));
        }
    }
}
//----------------------------------------------------------------------------------------------
vector<GoalEx*> TrainForceGoal::GetSucceededInstances(RtsGame &game)
{
    vector<GoalEx*> succeededInstances(m_succeededInstances);
    m_succeededInstances.clear();
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
unsigned TrainForceGoal::Hash()
{
    return StepTypeId() + _params[PARAM_EntityClassId] + _params[PARAM_Amount];
}