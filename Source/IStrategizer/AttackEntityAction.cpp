#include "AttackEntityAction.h"

#include <cassert>
#include "Vector2.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "AbstractAdapter.h"
#include "CellFeature.h"
#include "CaseBasedReasonerEx.h"
#include "DataMessage.h"
#include "EngineAssist.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameTechTree.h"
#include "GameType.h"
#include "GameEntity.h"
#include "AdapterEx.h"
#include "EntityClassExist.h"
#include "And.h"
#include "Not.h"

using namespace IStrategizer;
using namespace Serialization;

AttackEntityAction::AttackEntityAction() : Action(ACTIONEX_AttackEntity)
{
    _params[PARAM_EntityClassId] = ECLASS_START;
    _params[PARAM_TargetEntityClassId] = ECLASS_START;
    CellFeature::Null().To(_params);
}
//----------------------------------------------------------------------------------------------
AttackEntityAction::AttackEntityAction(const PlanStepParameters& p_parameters) : Action(ACTIONEX_AttackEntity, p_parameters)
{
}
//----------------------------------------------------------------------------------------------
void AttackEntityAction::Copy(IClonable* p_dest)
{
    Action::Copy(p_dest);
}
//----------------------------------------------------------------------------------------------
bool AttackEntityAction::ExecuteAux(RtsGame& game, const WorldClock& p_clock)
{
    EntityClassType attackerType = (EntityClassType)_params[PARAM_EntityClassId];
    EntityClassType targetType = (EntityClassType)_params[PARAM_TargetEntityClassId];
    AbstractAdapter *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();
    bool executed = false;
    
    // Adapt attacker
    m_attackerId = pAdapter->GetEntityObjectId(attackerType,AdapterEx::AttackerStatesRankVector);

    if (m_attackerId != INVALID_TID)
    {
        m_targetId = pAdapter->AdaptTargetEntity(targetType, Parameters());

        if (m_targetId != INVALID_TID)
        {
            GameEntity* pGameAttacker = game.Self()->GetEntity(m_attackerId);
            GameEntity* pGameTarget = game.Enemy()->GetEntity(m_targetId);
            _ASSERTE(pGameAttacker);
            _ASSERTE(pGameTarget);
            pGameAttacker->Lock(this);
            executed = pGameAttacker->AttackEntity(m_targetId);
        }
    }

    return executed;
}
//----------------------------------------------------------------------------------------------
bool AttackEntityAction::AliveConditionsSatisfied(RtsGame& game)
{
    bool attackerExists = g_Assist.DoesEntityObjectExist(m_attackerId);
    bool targetExists = g_Assist.DoesEntityObjectExist(m_targetId, PLAYER_Enemy);

    if (!attackerExists)
    {
        ConditionEx* failedCondition = new EntityClassExist(
            PLAYER_Self,
            (EntityClassType)_params[PARAM_EntityClassId],
            1,
            true);
        m_history.Add(ESTATE_Failed, failedCondition);
    }

    return attackerExists && targetExists;
}
//----------------------------------------------------------------------------------------------
bool AttackEntityAction::SuccessConditionsSatisfied(RtsGame& game)
{
    _ASSERTE(PlanStepEx::State() == ESTATE_Executing);

    GameEntity* pGameTarget = game.Enemy()->GetEntity(m_targetId);
    _ASSERTE(pGameTarget);

    ObjectStateType targetState = (ObjectStateType)pGameTarget->Attr(EOATTR_State);
    return targetState == OBJSTATE_UnderAttack;
}
//----------------------------------------------------------------------------------------------
void AttackEntityAction::InitializeAddressesAux()
{
    Action::InitializeAddressesAux();
}
//----------------------------------------------------------------------------------------------
void AttackEntityAction::InitializePostConditions()
{
    EntityClassType target = (EntityClassType)_params[PARAM_TargetEntityClassId];
    _postCondition = new Not(new EntityClassExist(PLAYER_Enemy, target, 1, true));
}
//----------------------------------------------------------------------------------------------
void AttackEntityAction::InitializePreConditions()
{
    vector<Expression*> m_terms;
    EntityClassType attacker = (EntityClassType)_params[PARAM_EntityClassId];
    EntityClassType target = (EntityClassType)_params[PARAM_TargetEntityClassId];

    m_terms.push_back(new EntityClassExist(PLAYER_Self, attacker, 1, true));
    m_terms.push_back(new EntityClassExist(PLAYER_Enemy, target, 1, false));
    _preCondition = new And(m_terms);
}