#include "TrainForceGoal.h"
#include "GoalFactory.h"
#include "BuildInfrastructureGoal.h"
#include "TrainArmyGoal.h"

using namespace IStrategizer;

//-------------------------------------------------------------------------------------------------------------------------
GoalEx* GoalFactory::GetGoal(GoalType p_goalType, const PlanStepParameters& p_parameters, bool p_initConditions)
{
    GoalEx* goal;

    switch(p_goalType)
    {
    case GOALEX_TrainForce:
        goal = new TrainForceGoal(p_parameters);
        break;

    case GOALEX_BuildInfrastructure:
        goal = new BuildInfrastructureGoal(p_parameters);
        break;

    case GOALEX_TrainArmy:
        goal = new TrainArmyGoal(p_parameters);
        break;

    default:
        DEBUG_THROW(InvalidParameterException(XcptHere));
    }

    if (p_initConditions)
    {
        goal->InitializeConditions();
        _ASSERTE(goal->PostCondition());
    }

    return goal;
}
//----------------------------------------------------------------------------------------------
GoalEx* GoalFactory::GetGoal(GoalType p_goalType, bool p_initConditions)
{
    GoalEx* goal;

    switch(p_goalType)
    {
    case GOALEX_TrainForce:
        goal = new TrainForceGoal();
        break;

    case GOALEX_BuildInfrastructure:
        goal = new BuildInfrastructureGoal();
        break;

    case GOALEX_TrainArmy:
        goal = new TrainArmyGoal();
        break;

    default:
        DEBUG_THROW(InvalidParameterException(XcptHere));
    }

    if (p_initConditions)
    {
        goal->InitializeConditions();
        _ASSERTE(goal->PostCondition());
    }

    return goal;
}
