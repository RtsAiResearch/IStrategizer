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
    _attackerId = pAdapter->GetEntityObjectId(attackerType,AdapterEx::AttackerStatesRankVector);

    if (_attackerId != INVALID_TID)
    {
        _targetId = pAdapter->AdaptTargetEntity(targetType, Parameters());

        if (_targetId != INVALID_TID)
        {
            GameEntity* pGameAttacker = game.Self()->GetEntity(_attackerId);
            GameEntity* pGameTarget = game.Enemy()->GetEntity(_targetId);
            assert(pGameAttacker);
            assert(pGameTarget);
            pGameAttacker->Lock(this);
            executed = pGameAttacker->AttackEntity(_targetId);
        }
    }

    return executed;
}
//----------------------------------------------------------------------------------------------
void AttackEntityAction::HandleMessage(RtsGame& game, Message* p_msg, bool& p_consumed)
{
    
}
//----------------------------------------------------------------------------------------------
bool AttackEntityAction::AliveConditionsSatisfied(RtsGame& game)
{
    return g_Assist.DoesEntityObjectExist(_attackerId) && g_Assist.DoesEntityObjectExist(_targetId, PLAYER_Enemy);
}
//----------------------------------------------------------------------------------------------
bool AttackEntityAction::SuccessConditionsSatisfied(RtsGame& game)
{
    assert(PlanStepEx::State() == ESTATE_Executing);

    GameEntity* pGameAttacker = game.Self()->GetEntity(_attackerId);
    GameEntity* pGameTarget = game.Enemy()->GetEntity(_targetId);
    assert(pGameAttacker);
    assert(pGameTarget);

    ObjectStateType attackerState = (ObjectStateType)pGameAttacker->Attr(EOATTR_State);
    ObjectStateType targetState = (ObjectStateType)pGameTarget->Attr(EOATTR_State);
    return (attackerState == OBJSTATE_Attacking) || (targetState == OBJSTATE_UnderAttack);
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