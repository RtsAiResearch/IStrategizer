#include "WinGameGoal.h"
#include "GoalFactory.h"
#include "RtsGame.h"
#include "TypesMetaData.h"
#include "GamePlayer.h"
#include "EntityClassExist.h"
#include "And.h"
#include "Not.h"
#include "Logger.h"

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
//----------------------------------------------------------------------------------------------
vector<GoalEx*> WinGameGoal::GetSucceededInstances(RtsGame &game)
{
    vector<GoalEx*> succeededGoals;
    vector<TID> enemyEntities;
    game.Enemy()->Entities(enemyEntities);

    if (enemyEntities.empty())
    {
        PlanStepParameters params;
        params[PARAM_StrategyTypeId] = STRTYPE_EarlyTierRush;
        succeededGoals.push_back(g_GoalFactory.GetGoal(GOALEX_WinGame, params, true));
        LogInfo("WinGameGoal succeeded with strategt type='%s'", Enums[params[PARAM_StrategyTypeId]]);
    }

    return succeededGoals;
}