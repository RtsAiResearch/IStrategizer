#include "DeployArmyGoal.h"

#include "RtsGame.h"
#include "TypesMetaData.h"
#include "GamePlayer.h"
#include "EntityClassExist.h"
#include "And.h"
#include <cassert>

using namespace IStrategizer;

DeployArmyGoal::DeployArmyGoal() : GoalEx(GOALEX_DeployArmy)
{
    _params[PARAM_DeployType] = DEPTYPE_START;
}
//----------------------------------------------------------------------------------------------
DeployArmyGoal::DeployArmyGoal(const PlanStepParameters& p_parameters): GoalEx(GOALEX_DeployArmy, p_parameters)
{
}
//----------------------------------------------------------------------------------------------
bool DeployArmyGoal::SuccessConditionsSatisfied(RtsGame* pRtsGame)
{
    return false;
}
//----------------------------------------------------------------------------------------------
void DeployArmyGoal::InitializePostConditions()
{
    
}