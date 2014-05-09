#include "WinGameGoal.h"
#include "DeployArmyGoal.h"
#include "DestroyEntityTypeGoal.h"
#include "CollectResourceGoal.h"
#include "TrainArmyGoal.h"
#include "GoalFactory.h"
#include "BuildInfrastructureGoal.h"

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

    case GOALEX_TrainArmy:
        goal = new TrainArmyGoal(p_parameters);
        break;

    case GOALEX_DeployArmy:
        goal = new DeployArmyGoal(p_parameters);
        break;

    case GOALEX_DestroyEntityType:
        goal = new DestroyEntityTypeGoal(p_parameters);
        break;

    case GOALEX_BuildInfrastructure:
        goal = new BuildInfrastructureGoal(p_parameters);
        break;

    default:
        DEBUG_THROW(InvalidParameterException(XcptHere));
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

    case GOALEX_TrainArmy:
        goal = new TrainArmyGoal();
        break;

    case GOALEX_DeployArmy:
        goal = new DeployArmyGoal();
        break;

    case GOALEX_DestroyEntityType:
        goal = new DestroyEntityTypeGoal();
        break;

    case GOALEX_BuildInfrastructure:
        goal = new BuildInfrastructureGoal();
        break;

    default:
        DEBUG_THROW(InvalidParameterException(XcptHere));
    }

    if(p_initConditions)
        goal->InitializeConditions();

    return goal;
}
