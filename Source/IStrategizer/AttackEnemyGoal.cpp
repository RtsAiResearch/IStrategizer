#include "AttackEnemyGoal.h"

#include "Not.h"
#include "And.h"
#include "EntityClassExist.h"
#include <cstdlib>
#include <ctime>
#include <cassert>

using namespace IStrategizer;

AttackEnemyGoal::AttackEnemyGoal() : GoalEx(GOALEX_AttackEnemy)
{
    /*_forceDescription = ForceDescriptionEx(FORCESIZE_SmallForce, PRCNT_0, PRCNT_0, PRCNT_100, PRCNT_0, PRCNT_0, PRCNT_0);
    _params[PARAM_ForceSizeId] = FORCESIZE_START;
    _params[PARAM_AttackTypeId] = ATTACK_START;*/
    InitializeConditions();
}
//----------------------------------------------------------------------------------------------
AttackEnemyGoal::AttackEnemyGoal(const PlanStepParameters& p_parameters): GoalEx(GOALEX_AttackEnemy, p_parameters)
{
     // FIXME: There should be a commander that specify force description in details
    //_forceDescription = ForceDescriptionEx(FORCESIZE_SmallForce, PRCNT_0, PRCNT_0, PRCNT_100, PRCNT_0, PRCNT_0, PRCNT_0);
    InitializeConditions();
}
//----------------------------------------------------------------------------------------------
void AttackEnemyGoal::InitializePostConditions()
{
    vector<Expression*> m_terms;

    //m_terms.push_back(new CheckEntityClassAttribute(PLAYER_Enemy, ECLASS_END, ECATTR_Count, RELOP_Equal, 0));

    //for (int index = 0; index < _params[PARAM_Value]; ++index)
    //{
    // //FIXME : LFHD use this condition
    // //m_terms.push_back(new CheckPositionFilterCount(PLAYER_Enemy, FILTER_AnyUnit, RELOP_Equal, 0, PositionFeatureVector::Null()));
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
//----------------------------------------------------------------------------------------------
bool AttackEnemyGoal::SuccessConditionsSatisfied(RtsGame* pRtsGame)
{
    return false;
}