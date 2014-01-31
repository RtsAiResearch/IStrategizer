#include "TrainArmyGoal.h"
#include "EntityClassExist.h"
#include "And.h"

using namespace IStrategizer;

TrainArmyGoal::TrainArmyGoal() : GoalEx(GOALEX_TrainArmy)
{
    _params[PARAM_ForceSizeId] = FORCESIZE_START;
}
//----------------------------------------------------------------------------------------------
TrainArmyGoal::TrainArmyGoal(const PlanStepParameters& p_parameters): GoalEx(GOALEX_TrainArmy, p_parameters)
{
}
//----------------------------------------------------------------------------------------------
void TrainArmyGoal::InitializePostConditions()
{
    vector<Expression*> m_terms;
    m_terms.push_back(new EntityClassExist(PLAYER_Self, (EntityClassType)_params[PARAM_EntityClassId]/*don't care of type #def engineData search*/, _params[PARAM_ForceSizeId], true));
    /*over EntityClassNearArea constructor to take filter Type*/
    _postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
bool TrainArmyGoal::SuccessConditionsSatisfied(RtsGame* pRtsGame)
{
    return false;
}