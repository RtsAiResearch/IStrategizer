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
	_targetCell = new CellFeature(p_parameters);
}
//----------------------------------------------------------------------------------------------
bool AttackGroundAction::ExecuteAux(const WorldClock& p_clock)
{
	EntityClassType		attackerType = (EntityClassType)_params[PARAM_EntityClassId];
	GameEntity			*pGameAttacker;
	AbstractAdapter		*pAdapter = g_OnlineCaseBasedPlanner->Reasoner()->Adapter();

	_attackerId = pAdapter->AdaptAttacker(attackerType);

	pGameAttacker = g_Game->Self()->GetEntity(_attackerId);
	assert(pGameAttacker);

	return true;

	//return pGameAttacker->AttackGround(7aga)

	////Vector2 targetPosition = g_OnlineCaseBasedPlanner->Reasoner()->Adapter()->AdaptPosition(_targetCell);
	//Vector2 targetPosition = Vector2::Null();
	//EntityClassExist* m_cond = (EntityClassExist*)_aliveCondition->operator [](0);

	//return g_Assist.ExecuteAttackGround(m_cond->Parameter(PARAM_EntityObjectId), targetPosition);
}
//----------------------------------------------------------------------------------------------
void AttackGroundAction::HandleMessage(Message* p_pMsg, bool& p_consumed)
{
	
}
//----------------------------------------------------------------------------------------------
bool AttackGroundAction::PreconditionsSatisfied()
{
	EntityClassType attacker = (EntityClassType)_params[PARAM_EntityClassId];
	bool success = false;

	g_Assist.EntityClassExist(MakePair(attacker, 1), success);

	if (!success)
		return false;
}
//----------------------------------------------------------------------------------------------
bool AttackGroundAction::AliveConditionsSatisfied()
{
	return true;
}
//----------------------------------------------------------------------------------------------
bool AttackGroundAction::SuccessConditionsSatisfied()
{
	throw NotImplementedException(XcptHere);
}
//----------------------------------------------------------------------------------------------
void  AttackGroundAction::InitializeAddressesAux()
{
	Action::InitializeAddressesAux();
	/*AddMemberAddress(1,
		&_targetCell);*/
}
//----------------------------------------------------------------------------------------------
void AttackGroundAction::InitializeSuccessConditions()
{
	//EntityClassExist* m_cond = (EntityClassExist*)_preCondition->operator [](0);
	//int	m_unitObjectId = m_cond->GetEntityIdByIndex(0);

	//_successCondition = new And();
	//_successCondition->AddExpression(new EntityObjectExist(PLAYER_Self, m_unitObjectId));
	//// FIXME: Target concrete position is not supported
	//_successCondition->AddExpression(new CheckEntityObjectAttribute(PLAYER_Self, m_unitObjectId, EOATTR_PosX, RELOP_Equal, 0 /*Target Concrete Location*/));
	//_successCondition->AddExpression(new CheckEntityObjectAttribute(PLAYER_Self, m_unitObjectId, EOATTR_PosY, RELOP_Equal, 0 /*Target Concrete Location*/));
}
//----------------------------------------------------------------------------------------------
void AttackGroundAction::InitializeAliveConditions()
{
	/*EntityClassExist* m_cond = (EntityClassExist*)_preCondition->operator [](0);
	int	m_unitObjectId = m_cond->GetEntityIdByIndex(0);

	_aliveCondition = new And();
	_aliveCondition->AddExpression(new EntityObjectExist(PLAYER_Self, m_unitObjectId));*/
}
//----------------------------------------------------------------------------------------------
void AttackGroundAction::InitializePreConditions()
{
    /*vector<Expression*> m_terms;

	m_terms.push_back(new EntityClassExist(PLAYER_Self, (EntityClassType)_params[PARAM_EntityClassId], 1, true, true));
    _preCondition = new And(m_terms);*/
}
//----------------------------------------------------------------------------------------------
void AttackGroundAction::InitializePostConditions()
{
    vector<Expression*> m_terms;

	//FIXME : LFHD use this condition
	//m_terms.push_back(new CheckPositionFilterCount((PlayerType)_params[PARAM_TargetPlayerId], FILTER_AnyUnit, RELOP_Equal, 0, PositionFeatureVector::Null()));
	_postCondition = new And(m_terms);
}
