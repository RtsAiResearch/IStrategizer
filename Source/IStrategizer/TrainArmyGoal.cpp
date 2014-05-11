#include "TrainArmyGoal.h"
#include "EntityClassExist.h"
#include "And.h"
#include "Message.h"
#include "DataMessage.h"
#include "GameEntity.h"
#include "DataMessage.h"
#include "GameEntity.h"
#include "GamePlayer.h"
#include "GameType.h"

using namespace IStrategizer;
using namespace std;

TrainArmyGoal::TrainArmyGoal() : GoalEx(GOALEX_TrainArmy)
{
    _params[PARAM_ForceSizeId] = FORCESIZE_START;
    _params[PARAM_EntityClassId] = ECLASS_START;
    m_trainedUnitsCount = 0;
}
//----------------------------------------------------------------------------------------------
TrainArmyGoal::TrainArmyGoal(const PlanStepParameters& p_parameters): GoalEx(GOALEX_TrainArmy, p_parameters)
{
}
//----------------------------------------------------------------------------------------------
void TrainArmyGoal::InitializePostConditions()
{
    vector<Expression*> m_terms;
    m_terms.push_back(new EntityClassExist(PLAYER_Self, (EntityClassType)_params[PARAM_EntityClassId], DONT_CARE, true));
    _postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
bool TrainArmyGoal::SuccessConditionsSatisfied(RtsGame& game)
{
    vector<TID> trainedUnits;

    for (size_t i = 0; i < m_pendingUnits.size(); ++i)
    {
        GameEntity *pEntity = game.Self()->GetEntity(m_pendingUnits[i]);

        if (pEntity)
        {
            ObjectStateType traineeState = (ObjectStateType)pEntity->Attr(EOATTR_State);
            bool traineeBeingTrained = traineeState == OBJSTATE_Idle;

            if (traineeBeingTrained)
            {
                m_trainedUnitsCount++;
                trainedUnits.push_back(pEntity->Id());
            }
        }
    }

    for (size_t i = 0; i < trainedUnits.size(); ++i)
    {
        m_pendingUnits.erase(std::remove(m_pendingUnits.begin(), m_pendingUnits.end(), trainedUnits[i]), m_pendingUnits.end());
    }

    m_demandUnitsCount = game.GetForceSizeCount((ForceSizeType)_params[PARAM_ForceSizeId]);

    return m_trainedUnitsCount >= m_demandUnitsCount;
}
//----------------------------------------------------------------------------------------------
void TrainArmyGoal::HandleMessage(RtsGame& game, Message* p_msg, bool& p_consumed)
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
        EntityClassType requiredEntityType = (EntityClassType)_params[PARAM_EntityClassId];
        if (entityType == requiredEntityType)
        {
            p_consumed = true;
            m_pendingUnits.push_back(entityId);
        }
    }
}
//----------------------------------------------------------------------------------------------
vector<GoalEx*> TrainArmyGoal::GetSucceededInstances(RtsGame &game)
{
    vector<GoalEx*> succeededInstances;
    vector<TID> entities;
    map<EntityClassType, int> newForces;

    game.Self()->Entities(entities);
    
    // Include new added units
    for (size_t i = 0; i < entities.size(); ++i)
    {
        if (find(m_usedUnits.begin(), m_usedUnits.end(), entities[i]) == m_usedUnits.end())
        {
            GameEntity* gameEntity = game.Self()->GetEntity(entities[i]);

            if (!game.GetEntityType(gameEntity->Type())->Attr(ECATTR_IsBuilding) &&
                 game.Self()->GetWorkerType() != gameEntity->Type())
            {
                newForces[game.Self()->GetEntity(entities[i])->Type()]++;
                m_usedUnits.push_back(gameEntity->Id());
            }
        }
    }

    PlanStepParameters params;

    // Find succeeded goals
    for (map<EntityClassType, int>::iterator itr = newForces.begin(); itr != newForces.end(); itr++)
    {
        params[PARAM_EntityClassId] = (*itr).first;
        params[PARAM_ForceSizeId] = game.GetForceSizeType((*itr).second + m_forces[(*itr).first]);
        succeededInstances.push_back(new TrainArmyGoal(params));
        m_forces[(*itr).first] += (*itr).second;

        LogInfo("TrainArmyGoal succeeded for entity type='%s' with force size='%s'",
            Enums[params[PARAM_EntityClassId]],
            Enums[params[PARAM_ForceSizeId]]);
    }

    
    return succeededInstances;
}