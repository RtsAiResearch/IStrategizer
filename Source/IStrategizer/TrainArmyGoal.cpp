#include "TrainArmyGoal.h"
#include "EntityClassExist.h"
#include "And.h"
#include "Message.h"
#include "DataMessage.h"
#include "GameEntity.h"
#include "Message.h"
#include "DataMessage.h"
#include "GameEntity.h"
#include "GamePlayer.h"

using namespace IStrategizer;
using namespace std;

TrainArmyGoal::TrainArmyGoal() : GoalEx(GOALEX_TrainArmy)
{
    _params[PARAM_ForceSizeId] = FORCESIZE_START;
    _params[PARAM_EntityClassId] = ECLASS_START;
    m_trainedUnitsCount = 0;
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
    m_terms.push_back(new EntityClassExist(PLAYER_Self, (EntityClassType)_params[PARAM_EntityClassId], _params[PARAM_ForceSizeId], true));
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
        assert(pMsg && pMsg->Data());

        if (pMsg->Data()->OwnerId != PLAYER_Self)
            return;

        TID entityId = pMsg->Data()->EntityId;
        GameEntity *pEntity = game.Self()->GetEntity(entityId);
        assert(pEntity);
        EntityClassType tempEntity = pEntity->Type();
        EntityClassType tempParam = (EntityClassType)_params[PARAM_EntityClassId];
        if (tempEntity == tempParam)
        {
            m_pendingUnits.push_back(entityId);
        }
    }
}