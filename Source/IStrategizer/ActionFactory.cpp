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

//-------------------------------------------------------------------------------------------------------
ActionFactory& ActionFactory::Instance(RtsGame& p_RtsGame)
{
    static ActionFactory m_instance;
    m_instance.m_RtsGame = &p_RtsGame;
    return m_instance;
}
//-------------------------------------------------------------------------------------------------------
ActionFactory& ActionFactory::Instance()
{
    static ActionFactory m_instance;
    m_instance.m_RtsGame = nullptr;
    return m_instance;
}
//-------------------------------------------------------------------------------------------------------
Action* ActionFactory::GetAction(ActionType p_actionType, const PlanStepParameters& p_paramaters) {
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
        throw exception("Action not supported");
    }

    if(m_RtsGame)
        m_action->InitializeConditions(*m_RtsGame);

    return m_action;
}
//-------------------------------------------------------------------------------------------------------
Action* ActionFactory::GetAction(ActionType p_actionType)
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
        throw exception("Action not supported");
    }

    if(m_RtsGame)
        m_action->InitializeConditions(*m_RtsGame);

    return m_action;
}
