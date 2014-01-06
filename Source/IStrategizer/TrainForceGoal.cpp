#include "TrainForceGoal.h"
#include "EntityClassExist.h"
#include "And.h"

using namespace IStrategizer;

TrainForceGoal::TrainForceGoal() : GoalEx(GOALEX_TrainForce)
{
    _params[PARAM_ForceSizeId] = FORCESIZE_START;
}
//----------------------------------------------------------------------------------------------
TrainForceGoal::TrainForceGoal(const PlanStepParameters& p_parameters): GoalEx(GOALEX_TrainForce, p_parameters)
{
}
//----------------------------------------------------------------------------------------------
void TrainForceGoal::InitializePostConditions()
{
    vector<Expression*> m_terms;

    m_terms.resize(_params[PARAM_Value]);
    for (int i = 0; i < _params[PARAM_Value]; ++i)
    {
        //FIXME : LFHD use this condition
        //m_terms[i] = new CheckPositionFilterCount(PLAYER_Self, FILTER_AttackingUnit, RELOP_Equal, 1, PositionFeatureVector::Null());
    }
    _postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
bool TrainForceGoal::SuccessConditionsSatisfied(RtsGame* pRtsGame)
{
    return false;
}