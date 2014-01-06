#include "WinGameGoal.h"

#include "RtsGame.h"
#include "TypesMetaData.h"
#include "GamePlayer.h"
#include "EntityClassExist.h"
#include "And.h"
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
bool WinGameGoal::SuccessConditionsSatisfied(RtsGame* pRtsGame)
{
    vector<TID> enemyEntities;
    pRtsGame->Enemy()->Entities(enemyEntities);

    // All enemy units are destroyed, win game!
    return enemyEntities.empty();
}
//----------------------------------------------------------------------------------------------
void WinGameGoal::InitializePostConditions()
{
    vector<Expression*> m_terms;
    m_terms.push_back(new EntityClassExist(PLAYER_Enemy));
    _postCondition = new And(m_terms);
}