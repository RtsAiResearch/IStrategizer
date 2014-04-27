#include "DestroyEntityTypeGoal.h"

#include "Not.h"
#include "And.h"
#include "EntityClassExist.h"
#include <cstdlib>
#include <ctime>
#include <cassert>
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameEntity.h"
#include "DataMessage.h"

using namespace IStrategizer;
using namespace std;

DestroyEntityTypeGoal::DestroyEntityTypeGoal() : GoalEx(GOALEX_DestroyEntityType)
{
    _params[PARAM_TargetEntityClassId] = ECLASS_START;
    _params[PARAM_Amount] = 0;
    m_demandTargetSize = 0;
}
//----------------------------------------------------------------------------------------------
DestroyEntityTypeGoal::DestroyEntityTypeGoal(const PlanStepParameters& p_parameters): GoalEx(GOALEX_DestroyEntityType, p_parameters)
{
}
//----------------------------------------------------------------------------------------------
void DestroyEntityTypeGoal::InitializePostConditions()
{
    EntityClassType targetType = (EntityClassType)_params[PARAM_TargetEntityClassId];
    
    std::vector<Expression*> expressions;
    expressions.push_back(new EntityClassExist(PLAYER_Enemy, targetType, 0, true));

    _postCondition = new And(expressions);
}   
//----------------------------------------------------------------------------------------------
void DestroyEntityTypeGoal::Copy(IClonable* p_dest)
{
    GoalEx::Copy(p_dest);
    DestroyEntityTypeGoal* m_dest = static_cast<DestroyEntityTypeGoal*>(p_dest);
    m_dest->m_demandTargetSize = this->m_demandTargetSize;
}
//----------------------------------------------------------------------------------------------
bool DestroyEntityTypeGoal::SuccessConditionsSatisfied(RtsGame& game)
{
    return m_demandTargetSize >= _params[PARAM_Amount];
}
//----------------------------------------------------------------------------------------------
void DestroyEntityTypeGoal::HandleMessage(RtsGame& game, Message* p_msg, bool& p_consumed)
{
    if (p_msg->MessageTypeID() == MSG_EntityDestroy)
    {
        EntityDestroyMessage* pMsg = static_cast<EntityDestroyMessage*>(p_msg);
        _ASSERTE(pMsg && pMsg->Data());

        if (pMsg->Data()->OwnerId != PLAYER_Enemy)
            return;

        m_destroyed[pMsg->Data()->EntityType]++;

        if (pMsg->Data()->EntityType == (EntityClassType)_params[PARAM_TargetEntityClassId])
        {
            m_demandTargetSize++;
        }
    }
}
//----------------------------------------------------------------------------------------------
vector<GoalEx*> DestroyEntityTypeGoal::GetSucceededInstances(RtsGame &game)
{
    vector<GoalEx*> succeededGoals;
    PlanStepParameters params;

    for (map<EntityClassType, int>::iterator itr = m_destroyed.begin(); itr != m_destroyed.end(); itr++)
    {
        params[PARAM_TargetEntityClassId] = (*itr).first;
        params[PARAM_Amount] = (*itr).second;
        succeededGoals.push_back(new DestroyEntityTypeGoal(params));

        LogInfo("DestroyEntityTypeGoal succeeded for entity type='%s' with amount='%d'",
            Enums[params[PARAM_TargetEntityClassId]],
            params[PARAM_Amount]);
    }

    m_destroyed.clear();

    return succeededGoals;
}