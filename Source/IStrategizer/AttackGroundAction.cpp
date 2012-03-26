#include "AttackGroundAction.h"
#include "EntityObjectExist.h"
#include "CheckEntityObjectAttribute.h"
#include "CheckPositionFilterCount.h"
#include "EntityClassExist.h"
#include "Misc.h"
#include "And.h"
#include "Vector2.h"
#include "OnlineCaseBasedPlannerEx.h"
#include "AbstractAdapter.h"
#include "CellFeature.h"
#include "CaseBasedReasonerEx.h"

using namespace OLCBP;

//----------------------------------------------------------------------------------------------
AttackGroundAction::AttackGroundAction() : Action(ACTIONEX_AttackGround)
{
	_params[PARAM_EntityClassId]	= ECLASS_START;
}
//----------------------------------------------------------------------------------------------
AttackGroundAction::AttackGroundAction(const PlanStepParameters& p_parameters, CellFeature* p_targetCell) : Action(ACTIONEX_AttackGround, p_parameters)
{
    _targetCell = p_targetCell;
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
	EntityClassExist* m_cond = (EntityClassExist*)_preCondition->operator [](0);
	int	m_unitObjectId = m_cond->GetEntityIdByIndex(0);

	_successCondition = new And();
	_successCondition->AddExpression(new EntityObjectExist(PLAYER_Self, m_unitObjectId));
	// FIXME: Target concrete position is not supported
	_successCondition->AddExpression(new CheckEntityObjectAttribute(PLAYER_Self, m_unitObjectId, EOATTR_PosX, RELOP_Equal, 0 /*Target Concrete Location*/));
	_successCondition->AddExpression(new CheckEntityObjectAttribute(PLAYER_Self, m_unitObjectId, EOATTR_PosY, RELOP_Equal, 0 /*Target Concrete Location*/));
}
//----------------------------------------------------------------------------------------------
void AttackGroundAction::InitializeAliveConditions()
{
	EntityClassExist* m_cond = (EntityClassExist*)_preCondition->operator [](0);
	int	m_unitObjectId = m_cond->GetEntityIdByIndex(0);

	_aliveCondition = new And();
	_aliveCondition->AddExpression(new EntityObjectExist(PLAYER_Self, m_unitObjectId));
}
//----------------------------------------------------------------------------------------------
void AttackGroundAction::InitializePreConditions()
{
    vector<Expression*> m_terms;

	m_terms.push_back(new EntityClassExist(PLAYER_Self, (EntityClassType)_params[PARAM_EntityClassId], 1, true, true));
    _preCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
void AttackGroundAction::InitializePostConditions()
{
    vector<Expression*> m_terms;

	//FIXME : LFHD use this condition
	//m_terms.push_back(new CheckPositionFilterCount((PlayerType)_params[PARAM_TargetPlayerId], FILTER_AnyUnit, RELOP_Equal, 0, PositionFeatureVector::Null()));
	_postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
int AttackGroundAction::ExecuteAux(unsigned long p_cycles)
{
	//Vector2 targetPosition = g_OnlineCaseBasedPlanner->Reasoner()->Adapter()->AdaptPosition(_targetCell);
	Vector2 targetPosition = Vector2::Null();
	EntityClassExist* m_cond = (EntityClassExist*)_aliveCondition->operator [](0);

	return g_Assist.ExecuteAttackGround(m_cond->Parameter(PARAM_EntityObjectId), targetPosition);
}
