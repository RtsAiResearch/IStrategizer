#include "AttackGroundAction.h"

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
#include "WorldMap.h"

using namespace IStrategizer;
using namespace Serialization;

//----------------------------------------------------------------------------------------------
AttackGroundAction::AttackGroundAction() : Action(ACTIONEX_AttackGround)
{
    _params[PARAM_EntityClassId] = ECLASS_START;
    CellFeature::Null().To(_params);
}
//----------------------------------------------------------------------------------------------
AttackGroundAction::AttackGroundAction(const PlanStepParameters& p_parameters) : Action(ACTIONEX_AttackGround, p_parameters)
{ 
}
//----------------------------------------------------------------------------------------------
bool AttackGroundAction::ExecuteAux(const WorldClock& p_clock)
{
    EntityClassType attackerType = (EntityClassType)_params[PARAM_EntityClassId];
    AbstractAdapter *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();
    bool executed = false;

    // Adapt attacker
    _attackerId = pAdapter->AdaptAttacker(attackerType);

    if (_attackerId != INVALID_TID)
    {
        GameEntity* pGameAttacker = g_Game->Self()->GetEntity(_attackerId);
        assert(pGameAttacker);
        pGameAttacker->Lock(this);

        // Adapt attack position
        _position = pAdapter->AdaptPosition(Parameters());
        executed = pGameAttacker->AttackGround(_position);
    }
    
    return executed;
}
//----------------------------------------------------------------------------------------------
void AttackGroundAction::HandleMessage(Message* p_pMsg, bool& p_consumed)
{
    
}
//----------------------------------------------------------------------------------------------
bool AttackGroundAction::PreconditionsSatisfied()
{
    EntityClassType attacker = (EntityClassType)_params[PARAM_EntityClassId];
    return g_Assist.DoesEntityClassExist(MakePair(attacker, 1));
}
//----------------------------------------------------------------------------------------------
bool AttackGroundAction::AliveConditionsSatisfied()
{
    return g_Assist.DoesEntityObjectExist(_attackerId);
}
//----------------------------------------------------------------------------------------------
bool AttackGroundAction::SuccessConditionsSatisfied()
{
    assert(PlanStepEx::State() == ESTATE_Executing);

    GameEntity* pGameAttacker = g_Game->Self()->GetEntity(_attackerId);
    assert(pGameAttacker);
    ObjectStateType attackerState = (ObjectStateType)pGameAttacker->Attr(EOATTR_State);
    return (attackerState == OBJSTATE_Attacking) || (attackerState == OBJSTATE_UnderAttack);
}
//----------------------------------------------------------------------------------------------
void  AttackGroundAction::InitializeAddressesAux()
{
    Action::InitializeAddressesAux();
}
