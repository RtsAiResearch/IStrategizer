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
bool AttackEntityAction::ExecuteAux(const WorldClock& p_clock)
{
    EntityClassType attackerType = (EntityClassType)_params[PARAM_EntityClassId];
    EntityClassType targetType = (EntityClassType)_params[PARAM_TargetEntityClassId];
    AbstractAdapter *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();
    bool executed = false;
    
    // Adapt attacker
    _attackerId = pAdapter->AdaptAttacker(attackerType);

    if (_attackerId != INVALID_TID)
    {
        _targetId = pAdapter->AdaptTargetEntity(targetType, Parameters());

        if (_targetId != INVALID_TID)
        {
            GameEntity* pGameAttacker = g_Game->Self()->GetEntity(_attackerId);
            GameEntity* pGameTarget = g_Game->Enemy()->GetEntity(_targetId);
            assert(pGameAttacker);
            assert(pGameTarget);
            pGameAttacker->Lock(this);
            executed = pGameAttacker->AttackEntity(_targetId);
        }
    }

    return executed;
}
//----------------------------------------------------------------------------------------------
void AttackEntityAction::HandleMessage(Message* p_pMsg, bool& p_consumed)
{
    
}
//----------------------------------------------------------------------------------------------
bool AttackEntityAction::PreconditionsSatisfied()
{
    EntityClassType attacker = (EntityClassType)_params[PARAM_EntityClassId];
    bool attackerTypeExists = g_Assist.DoesEntityClassExist(MakePair(attacker, 1));

    EntityClassType target = (EntityClassType)_params[PARAM_TargetEntityClassId];
    bool targetTypeExists = g_Assist.DoesEntityClassExist(MakePair(target, 1), PLAYER_Enemy);

    return attackerTypeExists && targetTypeExists;
}
//----------------------------------------------------------------------------------------------
bool AttackEntityAction::AliveConditionsSatisfied()
{
    return g_Assist.DoesEntityObjectExist(_attackerId);
}
//----------------------------------------------------------------------------------------------
bool AttackEntityAction::SuccessConditionsSatisfied()
{
    assert(PlanStepEx::State() == ESTATE_Executing);

    GameEntity* pGameAttacker = g_Game->Self()->GetEntity(_attackerId);
    GameEntity* pGameTarget = g_Game->Enemy()->GetEntity(_targetId);
    assert(pGameAttacker);
    assert(pGameTarget);

    ObjectStateType attackerState = (ObjectStateType)pGameAttacker->Attr(EOATTR_State);
    ObjectStateType targetState = (ObjectStateType)pGameTarget->Attr(EOATTR_State);
    return (attackerState == OBJSTATE_Attacking) || (targetState == OBJSTATE_UnderAttack);
}
//----------------------------------------------------------------------------------------------
void  AttackEntityAction::InitializeAddressesAux()
{
    Action::InitializeAddressesAux();
}
