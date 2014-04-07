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

TrainArmyGoal::TrainArmyGoal() : GoalEx(GOALEX_TrainArmy)
{
    _params[PARAM_ForceSizeId] = FORCESIZE_START;
    _params[PARAM_EntityClassId] = ECLASS_START;
    _demandSize = 0;
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
    /*over EntityClassNearArea constructor to take filter Type*/
    _postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
bool TrainArmyGoal::SuccessConditionsSatisfied(RtsGame& pRtsGame)
{    
    return _demandSize >= pRtsGame.GetForceSizeCount((ForceSizeType)_params[PARAM_ForceSizeId]);
}
//----------------------------------------------------------------------------------------------
void TrainArmyGoal::HandleMessage(RtsGame& pRtsGame, Message* p_msg, bool& p_consumed)
{
    if ( p_msg->MessageTypeID() == MSG_EntityCreate)
    {
        EntityCreateMessage* pMsg = static_cast<EntityCreateMessage*>(p_msg);
        assert(pMsg && pMsg->Data());

        if (pMsg->Data()->OwnerId != PLAYER_Self)
            return;

        TID entityId = pMsg->Data()->EntityId;
        GameEntity *pEntity = pRtsGame.Self()->GetEntity(entityId);
        assert(pEntity);
        EntityClassType tempEntity = pEntity->Type();
        EntityClassType tempParam = (EntityClassType)_params[PARAM_EntityClassId];
        if (tempEntity == tempParam)
        {
            _demandSize++;
        }
    }
}