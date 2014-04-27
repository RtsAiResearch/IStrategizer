#include "ResearchAction.h"
#include "BuildActionEx.h"
#include "TrainAction.h"
#include "AttackGroundAction.h"
#include "AttackEntityAction.h"
#include "MoveEntityAction.h"
#include "ActionFactory.h"
#include "CellFeature.h"
#include "MoveAction.h"

using namespace IStrategizer;
using namespace std;

Action* ActionFactory::GetAction(ActionType p_actionType, const PlanStepParameters& p_paramaters, bool p_initConditions) {
    Action* m_action = nullptr;

    switch(p_actionType)
    {
    case ACTIONEX_Research:
        m_action = new ResearchAction(p_paramaters);

        break;

    case ACTIONEX_Build:
        m_action = new BuildActionEx(p_paramaters);
        break;

    case ACTIONEX_Train:
        return new TrainAction(p_paramaters);
        break;

    case ACTIONEX_AttackGround:
        m_action = new AttackGroundAction(p_paramaters);
        break;

    case ACTIONEX_Move:
        m_action = new MoveAction(p_paramaters);
        break;

    case ACTIONEX_AttackEntity:
        return new AttackEntityAction(p_paramaters);
         break;

    default:
        _ASSERTE(!"Action not supported");
    }

    if(p_initConditions)
        m_action->InitializeConditions();

    return m_action;
}
//-------------------------------------------------------------------------------------------------------
Action* ActionFactory::GetAction(ActionType p_actionType, bool p_initConditions)
{
    Action* m_action = nullptr;

    switch(p_actionType)
    {
    case ACTIONEX_Research:
        m_action = new ResearchAction();
        break;

    case ACTIONEX_Build:
        m_action = new BuildActionEx();
        break;

    case ACTIONEX_Train:
        m_action = new TrainAction();
        break;

    case ACTIONEX_AttackGround:
        m_action = new AttackGroundAction();
        break;

    case ACTIONEX_Move:
        m_action = new MoveAction();
        break;

    case ACTIONEX_AttackEntity:
        m_action = new AttackEntityAction();
        break;

    default:
        _ASSERTE(!"Action not supported");
    }

    if(p_initConditions)
        m_action->InitializeConditions();

    return m_action;
}
