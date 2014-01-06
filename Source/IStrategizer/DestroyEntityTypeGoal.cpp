#include "DestroyEntityTypeGoal.h"

#include "Not.h"
#include "And.h"
#include "EntityClassExist.h"
#include <cstdlib>
#include <ctime>
#include <cassert>

using namespace IStrategizer;

DestroyEntityTypeGoal::DestroyEntityTypeGoal() : GoalEx(GOALEX_DestroyEntityType)
{
    /*_forceDescription = ForceDescriptionEx(FORCESIZE_SmallForce, PRCNT_0, PRCNT_0, PRCNT_100, PRCNT_0, PRCNT_0, PRCNT_0);
    _params[PARAM_ForceSizeId] = FORCESIZE_START;
    _params[PARAM_AttackTypeId] = ATTACK_START;*/
}
//----------------------------------------------------------------------------------------------
DestroyEntityTypeGoal::DestroyEntityTypeGoal(const PlanStepParameters& p_parameters): GoalEx(GOALEX_DestroyEntityType, p_parameters)
{
     // FIXME: There should be a commander that specify force description in details
    //_forceDescription = ForceDescriptionEx(FORCESIZE_SmallForce, PRCNT_0, PRCNT_0, PRCNT_100, PRCNT_0, PRCNT_0, PRCNT_0);
}
//----------------------------------------------------------------------------------------------
void DestroyEntityTypeGoal::InitializePostConditions()
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
void DestroyEntityTypeGoal::Copy(IClonable* p_dest)
{
    GoalEx::Copy(p_dest);

    DestroyEntityTypeGoal* m_dest = static_cast<DestroyEntityTypeGoal*>(p_dest);

    m_dest->_forceDescription   = _forceDescription;
    _cachedColony.Copy(&m_dest->_cachedColony);
}
//----------------------------------------------------------------------------------------------
bool DestroyEntityTypeGoal::SuccessConditionsSatisfied(RtsGame* pRtsGame)
{
    return false;
}