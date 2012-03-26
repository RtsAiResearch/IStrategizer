#include "WinWargusGoalEx.h"
#include "AttackEnemyGoal.h"
#include "BuildBaseGoal.h"
#include "TrainForceGoal.h"
#include "GoalFactory.h"

//-------------------------------------------------------------------------------------------------------------------------
GoalEx* GoalFactory::GetGoal(GoalType p_goalType, PlanStepParameters& p_parameters, bool p_initConditions)
{
	GoalEx* goal;

	switch(p_goalType)
	{
	case GOALEX_BuildBase:
		goal = new BuildBaseGoal(p_parameters);
		break;

	case GOALEX_WinGame:
		goal = new WinWargusGoalEx(p_parameters);
		break;

	case GOALEX_TrainForce:
		goal = new TrainForceGoal(p_parameters);
		break;

	case GOALEX_AttackEnemy:
		goal = new AttackEnemyGoal(p_parameters);
		break;

	default:
		throw exception("Not supported goal");
	}

	if(p_initConditions)
		goal->InitializeConditions();

	return goal;
}
//----------------------------------------------------------------------------------------------
GoalEx* GoalFactory::GetGoal(GoalType p_goalType, bool p_initConditions)
{
	int m_index = 0;
	GoalEx* goal;

	switch(p_goalType)
	{
	case GOALEX_BuildBase:
		goal = new BuildBaseGoal();
		break;

	case GOALEX_WinGame:
		goal = new WinWargusGoalEx();
		break;

	case GOALEX_TrainForce:
		goal = new TrainForceGoal();
		break;

	case GOALEX_AttackEnemy:
		goal = new AttackEnemyGoal();
		break;

	default:
		throw exception("Not supported goal");
	}

	if(p_initConditions)
		goal->InitializeConditions();

	return goal;
}