#include "DeployArmyGoal.h"

#include "RtsGame.h"
#include "TypesMetaData.h"
#include "GamePlayer.h"
#include "EntityClassExist.h"
#include "And.h"
#include <cassert>
#include "EntityClassNearArea.h"
#include "CellFeature.h"

using namespace IStrategizer;

DeployArmyGoal::DeployArmyGoal() : GoalEx(GOALEX_DeployArmy)
{
    _params[PARAM_DeployType] = DEPTYPE_START;
    _params[PARAM_EntityClassId] = ECLASS_START;
}
//----------------------------------------------------------------------------------------------
DeployArmyGoal::DeployArmyGoal(const PlanStepParameters& p_parameters): GoalEx(GOALEX_DeployArmy, p_parameters)
{
}
//----------------------------------------------------------------------------------------------
bool DeployArmyGoal::SuccessConditionsSatisfied(RtsGame& game)
{
    //Access worldMap getNearestEnemyBorders (10 for example)
    //check the number of allied units in these cells 
    //if equals to or greater than force size
    //      success 
    //else 
    //      failed
    return false;
}
//----------------------------------------------------------------------------------------------
void DeployArmyGoal::InitializePostConditions()
{


    vector<Expression*> m_terms;
   // m_terms.push_back(new EntityClassNearArea(PLAYER_Self,FILTER_AttackingUnit, new CellFeature(_params), 0, _params[PARAM_ForceSizeId]));
    /*over EntityClassNearArea constructor to take filter Type*/
    _postCondition = new And(m_terms);
}