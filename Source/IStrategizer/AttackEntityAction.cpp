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
	_params[PARAM_NumberOfPrimaryResources] = 0;
	_params[PARAM_NumberOfSecondaryResources] = 0;
	_params[PARAM_NumberOfSupplyResources] = 0;
	_params[PARAM_EnemyUnitsCount] = 0;
	_params[PARAM_EnemyUnitsTotalHP] = 0;
	_params[PARAM_EnemyUnitsTotalDamage] = 0;
	_params[PARAM_AlliedUnitsCount] = 0;
	_params[PARAM_AlliedUnitsTotalHP] = 0;
	_params[PARAM_AlliedUnitsTotalDamage] = 0;
	_params[PARAM_EnemyBuildingsCount] = 0;
	_params[PARAM_EnemyCriticalBuildingsCount] = 0;
	_params[PARAM_AlliedBuildingsCount] = 0;
	_params[PARAM_AlliedCriticalBuildingsCount] = 0;
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
	
	// Adapt attacker
	_attackerId = pAdapter->AdaptAttacker(attackerType);
	_targetId = pAdapter->AdaptTargetEntity(targetType, Parameters());
	_pGameAttacker = g_Game->Self()->GetEntity(_attackerId);
	assert(_pGameAttacker);

	return _pGameAttacker->AttackEntity(_targetId);
}
//----------------------------------------------------------------------------------------------
void AttackEntityAction::HandleMessage(Message* p_pMsg, bool& p_consumed)
{
	
}
//----------------------------------------------------------------------------------------------
bool AttackEntityAction::PreconditionsSatisfied()
{
	bool success = false;

	EntityClassType attacker = (EntityClassType)_params[PARAM_EntityClassId];
	success = g_Assist.DoesEntityClassExist(MakePair(attacker, 1));

	if (!success)
		return false;

	EntityClassType target = (EntityClassType)_params[PARAM_TargetEntityClassId];
	success = g_Assist.DoesEntityClassExist(MakePair(target, 1), PLAYER_Enemy);

	if (!success)
		return false;
    else
        return true;
}
//----------------------------------------------------------------------------------------------
bool AttackEntityAction::AliveConditionsSatisfied()
{
	bool success = false;

	EntityClassType attacker = (EntityClassType)_params[PARAM_EntityClassId];
	success = g_Assist.DoesEntityClassExist(MakePair(attacker, 1));

	if (!success)
		return false;
    else
        return true;
}
//----------------------------------------------------------------------------------------------
bool AttackEntityAction::SuccessConditionsSatisfied()
{
	bool success = false;

	EntityClassType target = (EntityClassType)_params[PARAM_TargetEntityClassId];
	success = g_Assist.DoesEntityClassExist(MakePair(target, 1), PLAYER_Enemy);

	if (!success)
		return true;
    else
        return false;
}
//----------------------------------------------------------------------------------------------
void  AttackEntityAction::InitializeAddressesAux()
{
	Action::InitializeAddressesAux();
}