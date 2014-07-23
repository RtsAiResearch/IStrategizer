#include "DestroyEntityTypeGoal.h"
#include <cstdlib>
#include <ctime>
#include <cassert>
#include "GamePlayer.h"
#include "GameEntity.h"
#include "DataMessage.h"
#include "Logger.h"
#include "GoalFactory.h"
#include "Not.h"
#include "And.h"
#include "EntityClassExist.h"
#include "RtsGame.h"

using namespace IStrategizer;
using namespace std;

DestroyEntityTypeGoal::DestroyEntityTypeGoal() : GoalEx(GOALEX_DestroyEntityType)
{
    _params[PARAM_TargetEntityClassId] = ECLASS_START;
    _params[PARAM_Amount] = DONT_CARE;
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
    _postCondition = new Not(new EntityClassExist(PLAYER_Enemy, targetType, _params[PARAM_Amount]));
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
    if (p_msg->TypeId() == MSG_EntityDestroy)
    {
        EntityDestroyMessage* pMsg = static_cast<EntityDestroyMessage*>(p_msg);
        _ASSERTE(pMsg && pMsg->Data());

        if (pMsg->Data()->OwnerId != PLAYER_Enemy)
            return;

        m_destroyed[pMsg->Data()->EntityType]++;
        PlanStepParameters params;
        params[PARAM_TargetEntityClassId] = pMsg->Data()->EntityType;
        params[PARAM_Amount] = m_destroyed[pMsg->Data()->EntityType];
        m_succededInstances.push_back(g_GoalFactory.GetGoal(GOALEX_DestroyEntityType, params, true));
        LogInfo("DestroyEntityTypeGoal succeeded for entity type='%s' with amount='%d'", Enums[params[PARAM_TargetEntityClassId]], params[PARAM_Amount]);

        if (pMsg->Data()->EntityType == (EntityClassType)_params[PARAM_TargetEntityClassId])
        {
            m_demandTargetSize++;
            p_consumed = true;
        }
    }
}
//----------------------------------------------------------------------------------------------
vector<GoalEx*> DestroyEntityTypeGoal::GetSucceededInstances(RtsGame &game)
{
    vector<GoalEx*> succeededGoals(m_succededInstances);
    m_succededInstances.clear();
    return succeededGoals;
}
//----------------------------------------------------------------------------------------------
bool DestroyEntityTypeGoal::Equals(PlanStepEx* p_planStep)
{
    return StepTypeId() == p_planStep->StepTypeId() &&
        _params[PARAM_TargetEntityClassId] == p_planStep->Parameter(PARAM_TargetEntityClassId) &&
        _params[PARAM_Amount] == p_planStep->Parameter(PARAM_Amount);
}
//----------------------------------------------------------------------------------------------
bool DestroyEntityTypeGoal::Merge(PlanStepEx* planStep)
{
    if (StepTypeId() == planStep->StepTypeId())
    {
        if (_params[PARAM_TargetEntityClassId] == planStep->Parameter(PARAM_TargetEntityClassId))
        {
            _params[PARAM_Amount] += planStep->Parameter(PARAM_Amount);
            return true;
        }
    }

    return false;
}