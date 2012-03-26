#include "AttackEntityAction.h"
#include "EntityClassExist.h"
#include "EntityObjectExist.h"
#include "CheckEntityObjectAttribute.h"
#include "CheckPositionFilterCount.h"
#include "Misc.h"
#include "And.h"

#ifndef RTSGAME_H
#include "RtsGame.h"
#endif
#ifndef GAMEPLAYER_H
#include "GamePlayer.h"
#endif
#ifndef GAMEENTITY_H
#include "GameEntity.h"
#endif
#ifndef OR_H
#include "Or.h"
#endif
#ifndef NOT_H
#include "Not.h"
#endif

AttackEntityAction::AttackEntityAction() : Action(ACTIONEX_AttackEntity)
{
	_params[PARAM_EntityClassId]		= ECLASS_START;
	_params[PARAM_TargetEntityClassId]	= ECLASS_START;
}
//----------------------------------------------------------------------------------------------
AttackEntityAction::AttackEntityAction(const PlanStepParameters& p_parameters, CellFeature *p_cellFeature) : Action(ACTIONEX_AttackEntity, p_parameters)
{
	_cellFeature = p_cellFeature;
}
//----------------------------------------------------------------------------------------------
void AttackEntityAction::InitializePreConditions()
{
	map< EntityObjectAttribute, int > m_targetAttributes;
	//FixMe : preConditions now don't check that enemy is available in "certain position"

	vector<Expression*> m_terms;
	int					aIndex;

	m_terms.resize(2);
	m_terms[aIndex = 0] = new EntityClassExist(PLAYER_Self, (EntityClassType)_params[PARAM_EntityClassId], 1, true, true);
	m_terms[++aIndex]   = new EntityClassExist(PLAYER_Enemy, (EntityClassType)_params[PARAM_TargetEntityClassId], 1, true);
	_preCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
void AttackEntityAction::InitializeAliveConditions()
{
	TID m_targetObjectId = ((EntityClassExist*)_preCondition->At(1))->EntityObjectIds()[0];
	int m_targetObjectHealth;

	g_Assist.GetEntityObjectAttribute(PLAYER_Enemy, m_targetObjectId, EOATTR_Health, m_targetObjectHealth);

	_aliveCondition->AddExpression(new EntityObjectExist(PLAYER_Self, ((EntityClassExist*)_preCondition->At(0))->EntityObjectIds()[0]));
	_aliveCondition->AddExpression(new EntityObjectExist(PLAYER_Enemy, m_targetObjectId));
}
//----------------------------------------------------------------------------------------------
void AttackEntityAction::InitializeSuccessConditions()
{
	TID m_targetObjectId = ((EntityClassExist*)_preCondition->At(1))->EntityObjectIds()[0];
	int m_targetObjectHealth;

	g_Assist.GetEntityObjectAttribute(PLAYER_Enemy, m_targetObjectId, EOATTR_Health, m_targetObjectHealth);

	_successCondition->AddExpression(new EntityObjectExist(PLAYER_Self, ((EntityClassExist*)_preCondition->At(0))->EntityObjectIds()[0]));
	Or* m_or = new Or();
	m_or->AddExpression(new Not(new EntityObjectExist(PLAYER_Enemy, m_targetObjectId)));
	m_or->AddExpression(new CheckEntityObjectAttribute(PLAYER_Enemy, m_targetObjectId, 
		EOATTR_Health, RELOP_LessThan, m_targetObjectHealth));
	_successCondition->AddExpression(m_or);
}
//----------------------------------------------------------------------------------------------
void AttackEntityAction::InitializePostConditions()
{
	vector<Expression*> m_terms;
	
	//FIXME : LFHD use this condition
	//m_terms.push_back(new CheckPositionFilterCount(PLAYER_Enemy, FILTER_AnyUnit, RELOP_Equal, 0, PositionFeatureVector::Null()));
	_postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
int AttackEntityAction::ExecuteAux(unsigned long p_cycles)
{
	return g_Assist.ExecuteAttackEntity(((ConditionEx*)_aliveCondition->At(0))->Parameter(PARAM_EntityObjectId), 
		PLAYER_Enemy, ((ConditionEx*)_aliveCondition->At(1))->Parameter(PARAM_EntityObjectId));
}
//----------------------------------------------------------------------------------------------
void AttackEntityAction::Update(unsigned long p_cycles)
{
	Action::Update(p_cycles);
	if(State() == ESTATE_Succeeded)
	{
		EntityClassExist* m_cond = (EntityClassExist*)_preCondition->operator [](0);
		GameEntity* m_entity = g_Game->GetPlayer(PLAYER_Self)->GetEntity(m_cond->GetEntityIdByIndex(0));
		assert(m_entity->Locked());
		m_entity->Unlock();
	}
}
//----------------------------------------------------------------------------------------------
void AttackEntityAction::Copy(IClonable* p_dest)
{
	Action::Copy(p_dest);

	AttackEntityAction* m_dest = static_cast<AttackEntityAction*>(p_dest);

    //_targetEntityPosDescription.Copy(&m_dest->_targetEntityPosDescription);
}