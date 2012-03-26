#include "Not.h"
#include "And.h"
#include "AttackEnemyGoal.h"
#include "EntityClassExist.h"
#include "EntityObjectExist.h"
#include "CheckEntityObjectAttribute.h"
#include "CheckPositionFilterCount.h"
#include "CheckColonyFilterCount.h"
#include "CheckEntityClassAttribute.h"
#include "Colony.h"
#include <cstdlib>
#include <ctime>
#include <cassert>

AttackEnemyGoal::AttackEnemyGoal() : GoalEx(GOALEX_AttackEnemy)
{
    _forceDescription				= ForceDescriptionEx(FORCESIZE_SmallForce, PRCNT_0, PRCNT_0, PRCNT_100, PRCNT_0, PRCNT_0, PRCNT_0);
	_params[PARAM_ForceSizeId]		= FORCESIZE_START;
	_params[PARAM_AttackTypeId]		= ATTACK_START;
}
//----------------------------------------------------------------------------------------------
AttackEnemyGoal::AttackEnemyGoal(const PlanStepParameters& p_parameters): GoalEx(GOALEX_AttackEnemy, p_parameters)
{
     // FIXME: There should be a commander that specify force description in details
    _forceDescription = ForceDescriptionEx(FORCESIZE_SmallForce, PRCNT_0, PRCNT_0, PRCNT_100, PRCNT_0, PRCNT_0, PRCNT_0);
}
//----------------------------------------------------------------------------------------------
void AttackEnemyGoal::InitializeSuccessConditions()
{
	int                 m_index;
	vector<Colony>      m_currentColonies;
	vector<Expression*> m_terms;

	//m_ret = g_EngineAPI.GetPlayerColonies(PLAYER_Enemy, m_currentColonies);
	//assert(m_ret == ERR_Success);
	//_cachedColony = m_currentColonies[0];

	switch(_params[PARAM_AttackTypeId])
	{
	case ATTACK_Defeat:
		m_terms.resize(1);
		m_terms[m_index = 0] = new CheckColonyFilterCount(PLAYER_Enemy, FILTER_AnyEntity, RELOP_Equal, 0, _cachedColony);
		break;

		// FIXME: it is assumed that the defender will not move until the attacker reach him
		// The defender should be changed if the chosen one went out of the colony
		//     case ATTACK_Attrition:
		//         vector<int> m_colonyObjectIds;
		//         m_ret = g_EngineAPI.GetPlayerColonyObjectsId(PLAYER_Enemy, _cachedColony, m_colonyObjectIds);
		//         assert(m_ret == ERR_Success);
		//         
		//         srand(time(NULL));
		//         int m_randomIdx = rand() % m_colonyObjectIds.size();
		//         int m_chosenTargetEntityObject = m_colonyObjectIds[m_randomIdx];
		// 
		//         m_terms[m_index = 0] = new Not(new EntityObjectExist(PLAYER_Enemy, m_chosenTargetEntityObject));
		//         break;
	}

	_successCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
void AttackEnemyGoal::InitializePostConditions()
{
	vector<Expression*> m_terms;

	//m_terms.push_back(new CheckEntityClassAttribute(PLAYER_Enemy, ECLASS_END, ECATTR_Count, RELOP_Equal, 0));

	//for (int index = 0; index < _params[PARAM_Value]; ++index)
	//{
	//	//FIXME : LFHD use this condition
	//	//m_terms.push_back(new CheckPositionFilterCount(PLAYER_Enemy, FILTER_AnyUnit, RELOP_Equal, 0, PositionFeatureVector::Null()));
	//}
	_postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
void AttackEnemyGoal::Copy(IClonable* p_dest)
{
    GoalEx::Copy(p_dest);

    AttackEnemyGoal* m_dest = static_cast<AttackEnemyGoal*>(p_dest);

    m_dest->_forceDescription   = _forceDescription;
    _cachedColony.Copy(&m_dest->_cachedColony);
}