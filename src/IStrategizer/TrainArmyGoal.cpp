
#include "TrainArmyGoal.h"
#include "PlayerAttributeExist.h"
#include "And.h"
#include "Message.h"
#include "DataMessage.h"
#include "GameEntity.h"
#include "DataMessage.h"
#include "GameEntity.h"
#include "GamePlayer.h"
#include "GameType.h"
#include "GoalFactory.h"

using namespace IStrategizer;
using namespace std;

TrainArmyGoal::TrainArmyGoal() :
    m_maxHP(0),
    m_maxDamage(0),
    GoalEx(GOALEX_TrainArmy)
{
    _params[PARAM_AlliedAttackersTotalHP] = DONT_CARE;
    _params[PARAM_AlliedAttackersTotalDamage] = DONT_CARE;
}
//----------------------------------------------------------------------------------------------
TrainArmyGoal::TrainArmyGoal(const PlanStepParameters& p_parameters) : 
    m_maxHP(0),
    m_maxDamage(0),
    GoalEx(GOALEX_TrainArmy, p_parameters)
{
}
//----------------------------------------------------------------------------------------------
void TrainArmyGoal::InitializePostConditions()
{
    vector<Expression*> m_terms;
    m_terms.push_back(new PlayerAttributeExist(PLAYER_Self, PATTR_AlliedAttackersTotalHP, _params[PARAM_AlliedAttackersTotalHP]));
    m_terms.push_back(new PlayerAttributeExist(PLAYER_Self, PATTR_AlliedAttackersTotalDamage, _params[PARAM_AlliedAttackersTotalDamage]));
    _postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
bool TrainArmyGoal::SuccessConditionsSatisfied(RtsGame& game)
{
    int hpAmount = game.Self()->Attr(PATTR_AlliedAttackersTotalHP);
    int damageAmount = game.Self()->Attr(PATTR_AlliedAttackersTotalDamage);

    return hpAmount >= _params[PARAM_AlliedAttackersTotalHP] &&
           damageAmount >= _params[PARAM_AlliedAttackersTotalDamage];
}
//----------------------------------------------------------------------------------------------
vector<GoalEx*> TrainArmyGoal::GetSucceededInstances(RtsGame &game)
{
    vector<GoalEx*> succeededInstances;
    
    int hpAmount = game.Self()->Attr(PATTR_AlliedAttackersTotalHP);
    int damageAmount = game.Self()->Attr(PATTR_AlliedAttackersTotalDamage);

    if (hpAmount > m_maxHP || damageAmount > m_maxDamage)
    {
        PlanStepParameters params;
        params[PARAM_AlliedAttackersTotalHP] = hpAmount;
        params[PARAM_AlliedAttackersTotalDamage] = damageAmount;
        succeededInstances.push_back(g_GoalFactory.GetGoal(GOALEX_TrainArmy, params));
        m_maxHP = hpAmount > m_maxHP ? hpAmount : m_maxHP;
        m_maxDamage = damageAmount > m_maxDamage ? damageAmount : m_maxDamage;
    }

    return succeededInstances;
}
//----------------------------------------------------------------------------------------------
bool TrainArmyGoal::Equals(PlanStepEx* p_planStep)
{
    return StepTypeId() == p_planStep->StepTypeId() &&
        _params[PARAM_AlliedAttackersTotalHP] == p_planStep->Parameter(PARAM_AlliedAttackersTotalHP) &&
        _params[PARAM_AlliedAttackersTotalDamage] == p_planStep->Parameter(PARAM_AlliedAttackersTotalDamage);
}
//----------------------------------------------------------------------------------------------
bool TrainArmyGoal::Merge(PlanStepEx* planStep)
{
    if (StepTypeId() == planStep->StepTypeId())
    {
        _params[PARAM_AlliedAttackersTotalHP] += planStep->Parameter(PARAM_AlliedAttackersTotalHP);
        _params[PARAM_AlliedAttackersTotalDamage] += planStep->Parameter(PARAM_AlliedAttackersTotalDamage);
        return true;
    }

    return false;
}