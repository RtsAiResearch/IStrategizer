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
AttackGroundAction::AttackGroundAction(const PlanStepParameters& p_parameters) : Action(ACTIONEX_AttackGround, p_parameters)
{	
}
//----------------------------------------------------------------------------------------------
bool AttackGroundAction::ExecuteAux(const WorldClock& p_clock)
{
	EntityClassType attackerType = (EntityClassType)_params[PARAM_EntityClassId];
	AbstractAdapter *pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();

    // Adapt attack position
    _position = pAdapter->AdaptPosition(Parameters());
	CellFeature* pAttackCell = g_Game->Map()->GetCellFeature(_position);
	
	_numberOfEnemyBuildings = pAttackCell->m_enemyBuildingDescription.m_numberOfBuildings;
	_numberOfEnemyUnits = pAttackCell->m_enemyForceDescription.m_numberOfUnits;

	// Adapt attacker
	_attackerId = pAdapter->AdaptAttacker(attackerType);
	_pGameAttacker = g_Game->Self()->GetEntity(_attackerId);
	assert(_pGameAttacker);
	
	return _pGameAttacker->AttackGround(_position.X, _position.Y);
}
//----------------------------------------------------------------------------------------------
void AttackGroundAction::HandleMessage(Message* p_pMsg, bool& p_consumed)
{
	
}
//----------------------------------------------------------------------------------------------
bool AttackGroundAction::PreconditionsSatisfied()
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
bool AttackGroundAction::AliveConditionsSatisfied()
{
	bool success = false;

	success = g_Assist.DoesEntityObjectExist(_attackerId);

	if (!success)
		return false;
    else
        return true;
}
//----------------------------------------------------------------------------------------------
bool AttackGroundAction::SuccessConditionsSatisfied()
{
	CellFeature* pEnemy = g_Game->Map()->GetCellFeature(_position);
	int numberOfEnemyBuildings = pEnemy->m_enemyBuildingDescription.m_numberOfBuildings;
	int numberOfEnemyUnits = pEnemy->m_enemyForceDescription.m_numberOfUnits;

	return numberOfEnemyBuildings < _numberOfEnemyBuildings || numberOfEnemyUnits < _numberOfEnemyUnits;
}
//----------------------------------------------------------------------------------------------
void  AttackGroundAction::InitializeAddressesAux()
{
	Action::InitializeAddressesAux();
}