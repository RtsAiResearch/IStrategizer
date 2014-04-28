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

AttackEntityAction::AttackEntityAction() :
    Action(ACTIONEX_AttackEntity)
{
    _params[PARAM_EntityClassId] = ECLASS_START;
    _params[PARAM_TargetEntityClassId] = ECLASS_START;
    CellFeature::Null().To(_params);
}
//----------------------------------------------------------------------------------------------
AttackEntityAction::AttackEntityAction(const PlanStepParameters& p_parameters) :
    Action(ACTIONEX_AttackEntity, p_parameters)
{
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
            GameEntity* pAttacker = game.Self()->GetEntity(m_attackerId);
            _ASSERTE(pAttacker);

            pAttacker->Lock(this);
            executed = pAttacker->AttackEntity(m_targetId);
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
    EntityClassType targetType = (EntityClassType)_params[PARAM_TargetEntityClassId];
    
    std::vector<Expression*> expressions;
    expressions.push_back(new EntityClassExist(PLAYER_Enemy, targetType, 0, true));

    _postCondition = new And(expressions);
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
//----------------------------------------------------------------------------------------------
void AttackEntityAction::OnSucccess(RtsGame& game, const WorldClock& p_clock)
{
    GameEntity* pAttacker = game.Self()->GetEntity(m_attackerId);

    if (pAttacker && pAttacker->IsLocked() && pAttacker->Owner() == this)
        pAttacker->Unlock(this);
}
//----------------------------------------------------------------------------------------------
void AttackEntityAction::OnFailure(RtsGame& game, const WorldClock& p_clock)
{
    GameEntity* pAttacker = game.Self()->GetEntity(m_attackerId);

    if (pAttacker && pAttacker->IsLocked() && pAttacker->Owner() == this)
        pAttacker->Unlock(this);
}