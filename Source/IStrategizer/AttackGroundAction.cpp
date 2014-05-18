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
#include "AdapterEx.h"
#include "EntityClassExist.h"
#include "Not.h"

using namespace IStrategizer;
using namespace Serialization;
using namespace std;

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
bool AttackGroundAction::ExecuteAux(RtsGame& game, const WorldClock& p_clock)
{
    EntityClassType attackerType = (EntityClassType)_params[PARAM_EntityClassId];
    AbstractAdapter *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();
    bool executed = false;

    // Adapt attacker
    _attackerId = pAdapter->GetEntityObjectId(attackerType,AdapterEx::AttackerStatesRank);

    if (_attackerId != INVALID_TID)
    {
        GameEntity* pGameAttacker = game.Self()->GetEntity(_attackerId);
        _ASSERTE(pGameAttacker);
        pGameAttacker->Lock(this);

        // Adapt attack position
        _position = pAdapter->AdaptEnemyBorder();
        executed = pGameAttacker->AttackGround(_position);
    }
    
    return executed;
}
//----------------------------------------------------------------------------------------------
bool AttackGroundAction::AliveConditionsSatisfied(RtsGame& game)
{
    bool attackerExists = g_Assist.DoesEntityObjectExist(_attackerId);

    if (!attackerExists)
    {
        ConditionEx* failedCondition = new EntityClassExist(PLAYER_Self, (EntityClassType)_params[PARAM_EntityClassId], 1);
        m_history.Add(ESTATE_Failed, failedCondition);
    }

    return attackerExists;
}
//----------------------------------------------------------------------------------------------
bool AttackGroundAction::SuccessConditionsSatisfied(RtsGame& game)
{
    _ASSERTE(PlanStepEx::State() == ESTATE_Executing);

    GameEntity* pGameAttacker = game.Self()->GetEntity(_attackerId);
    _ASSERTE(pGameAttacker);
    ObjectStateType attackerState = (ObjectStateType)pGameAttacker->Attr(EOATTR_State);
    return (attackerState == OBJSTATE_Attacking) || (attackerState == OBJSTATE_UnderAttack);
}
//----------------------------------------------------------------------------------------------
void  AttackGroundAction::InitializeAddressesAux()
{
    Action::InitializeAddressesAux();
}
//----------------------------------------------------------------------------------------------
void AttackGroundAction::InitializePostConditions()
{
    _postCondition = new Not(new EntityClassExist(PLAYER_Enemy, 1));
}
//----------------------------------------------------------------------------------------------
void AttackGroundAction::InitializePreConditions()
{
    vector<Expression*> m_terms;
    EntityClassType attacker = (EntityClassType)_params[PARAM_EntityClassId];
    m_terms.push_back(new EntityClassExist(PLAYER_Self, attacker, 1));
    _preCondition = new And(m_terms);
}