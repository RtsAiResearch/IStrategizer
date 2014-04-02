#include "WinGameGoal.h"

#include "RtsGame.h"
#include "TypesMetaData.h"
#include "GamePlayer.h"
#include "EntityClassExist.h"
#include "And.h"
#include "Not.h"
#include <cassert>

using namespace IStrategizer;

WinGameGoal::WinGameGoal() : GoalEx(GOALEX_WinGame)
{
    _params[PARAM_StrategyTypeId] = STRTYPE_START;
}
//----------------------------------------------------------------------------------------------
WinGameGoal::WinGameGoal(const PlanStepParameters& p_parameters): GoalEx(GOALEX_WinGame, p_parameters)
{
}
//----------------------------------------------------------------------------------------------
bool WinGameGoal::SuccessConditionsSatisfied(RtsGame& game)
{
    vector<TID> enemyEntities;
    game.Enemy()->Entities(enemyEntities);

    // All enemy units are destroyed, win game!
    return enemyEntities.empty();
}
//----------------------------------------------------------------------------------------------
void WinGameGoal::InitializePostConditions()
{
    _postCondition = new Not(new EntityClassExist(PLAYER_Enemy));
}