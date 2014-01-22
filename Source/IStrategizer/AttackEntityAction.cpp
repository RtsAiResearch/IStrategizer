#include <cassert>
#include "AttackEntityAction.h"
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
bool AttackEntityAction::ExecuteAux(RtsGame& p_RtsGame, const WorldClock& p_clock)
{
    EntityClassType attackerType = (EntityClassType)_params[PARAM_EntityClassId];
    EntityClassType targetType = (EntityClassType)_params[PARAM_TargetEntityClassId];
    AbstractAdapter *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();
    bool executed = false;
    
    // Adapt attacker
    _attackerId = pAdapter->GetEntityObjectId(p_RtsGame, attackerType,AdapterEx::AttackerStatesRankVector);

    if (_attackerId != INVALID_TID)
    {
        _targetId = pAdapter->AdaptTargetEntity(p_RtsGame, targetType, Parameters());

        if (_targetId != INVALID_TID)
        {
            GameEntity* pGameAttacker = p_RtsGame.Self()->GetEntity(_attackerId);
            GameEntity* pGameTarget = p_RtsGame.Enemy()->GetEntity(_targetId);
            assert(pGameAttacker);
            assert(pGameTarget);
            pGameAttacker->Lock(this);
            executed = pGameAttacker->AttackEntity(_targetId);
        }
    }

    return executed;
}
//----------------------------------------------------------------------------------------------
void AttackEntityAction::HandleMessage(RtsGame& p_RtsGame, Message* p_msg, bool& p_consumed)
{
    
}
//----------------------------------------------------------------------------------------------
bool AttackEntityAction::AliveConditionsSatisfied(RtsGame& p_RtsGame)
{
    return EngineAssist::Instance(&p_RtsGame).DoesEntityObjectExist(_attackerId);
}
//----------------------------------------------------------------------------------------------
bool AttackEntityAction::SuccessConditionsSatisfied(RtsGame& p_RtsGame)
{
    assert(PlanStepEx::State() == ESTATE_Executing);

    GameEntity* pGameAttacker = p_RtsGame.Self()->GetEntity(_attackerId);
    GameEntity* pGameTarget = p_RtsGame.Enemy()->GetEntity(_targetId);
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
    EntityClassType target = (EntityClassType)_params[PARAM_TargetEntityClassId];
    _postCondition = new Not(new EntityClassExist(PLAYER_Enemy, target, 1, true));
}
//----------------------------------------------------------------------------------------------
void AttackEntityAction::InitializePreConditions(RtsGame& p_RtsGame)
{
    vector<Expression*> m_terms;
    EntityClassType attacker = (EntityClassType)_params[PARAM_EntityClassId];
    EntityClassType target = (EntityClassType)_params[PARAM_TargetEntityClassId];

    m_terms.push_back(new EntityClassExist(PLAYER_Self, attacker, 1, true));
    m_terms.push_back(new EntityClassExist(PLAYER_Enemy, target, 1, false));
    _preCondition = new And(m_terms);
}
