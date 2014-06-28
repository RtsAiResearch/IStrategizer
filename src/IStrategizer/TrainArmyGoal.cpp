
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
    _params[PARAM_AlliedUnitsTotalHP] = DONT_CARE;
    _params[PARAM_AlliedUnitsTotalDamage] = DONT_CARE;
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
    m_terms.push_back(new PlayerAttributeExist(PLAYER_Self, PATTR_AlliedUnitsTotalHP, _params[PARAM_AlliedUnitsTotalHP]));
    m_terms.push_back(new PlayerAttributeExist(PLAYER_Self, PATTR_AlliedUnitsTotalDamage, _params[PARAM_AlliedUnitsTotalDamage]));
    _postCondition = new And(m_terms);
}
//----------------------------------------------------------------------------------------------
bool TrainArmyGoal::SuccessConditionsSatisfied(RtsGame& game)
{
    int hpAmount = game.Self()->Attr(PATTR_AlliedUnitsTotalHP);
    int damageAmount = game.Self()->Attr(PATTR_AlliedUnitsTotalDamage);

    return hpAmount >= _params[PARAM_AlliedUnitsTotalHP] &&
           damageAmount >= _params[PARAM_AlliedUnitsTotalDamage];
}
//----------------------------------------------------------------------------------------------
vector<GoalEx*> TrainArmyGoal::GetSucceededInstances(RtsGame &game)
{
    vector<GoalEx*> succeededInstances;
    
    int hpAmount = game.Self()->Attr(PATTR_AlliedUnitsTotalHP);
    int damageAmount = game.Self()->Attr(PATTR_AlliedUnitsTotalDamage);

    if (hpAmount > m_maxHP || damageAmount > m_maxDamage)
    {
        PlanStepParameters params;
        params[PARAM_AlliedUnitsTotalHP] = hpAmount;
        params[PARAM_AlliedUnitsTotalDamage] = damageAmount;
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
        _params[PARAM_AlliedUnitsTotalHP] == p_planStep->Parameter(PARAM_AlliedUnitsTotalHP) &&
        _params[PARAM_AlliedUnitsTotalDamage] == p_planStep->Parameter(PARAM_AlliedUnitsTotalDamage);
}
//----------------------------------------------------------------------------------------------
bool TrainArmyGoal::Merge(PlanStepEx* planStep)
{
    if (StepTypeId() == planStep->StepTypeId())
    {
        _params[PARAM_AlliedUnitsTotalHP] += planStep->Parameter(PARAM_AlliedUnitsTotalHP);
        _params[PARAM_AlliedUnitsTotalDamage] += planStep->Parameter(PARAM_AlliedUnitsTotalDamage);
        return true;
    }

    return false;
}