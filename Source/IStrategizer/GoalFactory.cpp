#include "WinGameGoal.h"
#include "DeployArmyGoal.h"
#include "DestroyEntityTypeGoal.h"
#include "CollectResourceGoal.h"
#include "TrainForceGoal.h"
#include "GoalFactory.h"

using namespace IStrategizer;

//-------------------------------------------------------------------------------------------------------------------------
GoalEx* GoalFactory::GetGoal(GoalType p_goalType, PlanStepParameters& p_parameters, bool p_initConditions)
{
    GoalEx* goal;

    switch(p_goalType)
    {
    case GOALEX_CollectResource:
        goal = new CollectResourceGoal(p_parameters);
        break;

    case GOALEX_WinGame:
        goal = new WinGameGoal(p_parameters);
        break;

    case GOALEX_TrainForce:
        goal = new TrainForceGoal(p_parameters);
        break;

    case GOALEX_DeployArmy:
        goal = new DeployArmyGoal(p_parameters);
        break;

    case GOALEX_DestroyEntityType:
        goal = new DestroyEntityTypeGoal(p_parameters);
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
    GoalEx* goal;

    switch(p_goalType)
    {
    case GOALEX_CollectResource:
        goal = new CollectResourceGoal();
        break;

    case GOALEX_WinGame:
        goal = new WinGameGoal();
        break;

    case GOALEX_TrainForce:
        goal = new TrainForceGoal();
        break;

    case GOALEX_DeployArmy:
        goal = new DeployArmyGoal();
        break;

    case GOALEX_DestroyEntityType:
        goal = new DestroyEntityTypeGoal();
        break;

    default:
        throw exception("Not supported goal");
    }

    if(p_initConditions)
        goal->InitializeConditions();

    return goal;
}
