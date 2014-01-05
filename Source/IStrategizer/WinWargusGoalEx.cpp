#include "WinWargusGoalEx.h"

#include "RtsGame.h"
#include "TypesMetaData.h"
#include "GamePlayer.h"
#include <cassert>

using namespace IStrategizer;

WinWargusGoalEx::WinWargusGoalEx() : GoalEx(GOALEX_WinGame)
{
    _params[PARAM_StrategyTypeId] = STRTYPE_START;
}
//----------------------------------------------------------------------------------------------
WinWargusGoalEx::WinWargusGoalEx(const PlanStepParameters& p_parameters): GoalEx(GOALEX_WinGame, p_parameters)
{
}
//----------------------------------------------------------------------------------------------
bool WinWargusGoalEx::SuccessConditionsSatisfied()
{
    return false;
}
//----------------------------------------------------------------------------------------------
void WinWargusGoalEx::InitializePostConditions()
{

}