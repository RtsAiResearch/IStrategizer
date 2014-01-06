#include "WinWargusGoalEx.h"

#include "RtsGame.h"
#include "TypesMetaData.h"
#include "GamePlayer.h"
#include <cassert>

using namespace IStrategizer;

WinWargusGoalEx::WinWargusGoalEx() : GoalEx(GOALEX_WinGame)
{
    _params[PARAM_StrategyTypeId] = STRTYPE_START;
    InitializeConditions();
}
//----------------------------------------------------------------------------------------------
WinWargusGoalEx::WinWargusGoalEx(const PlanStepParameters& p_parameters): GoalEx(GOALEX_WinGame, p_parameters)
{
    InitializeConditions();
}
//----------------------------------------------------------------------------------------------
bool WinWargusGoalEx::SuccessConditionsSatisfied(RtsGame* pRtsGame)
{
    return false;
}
//----------------------------------------------------------------------------------------------
void WinWargusGoalEx::InitializePostConditions()
{

}