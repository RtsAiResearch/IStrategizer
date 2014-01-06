#include "Action.h"

#ifndef AND_H
#include "And.h"
#endif
#ifndef LOGGER_H
#include "Logger.h"
#endif
#ifndef COMPOSITEEXPRESSION_H
#include "CompositeExpression.h"
#endif
#ifndef RTSGAME_H
#include "RtsGame.h"
#endif
#ifndef TURE_H
#include "True.h"
#endif
#include <cassert>

using namespace IStrategizer;

Action::Action(ActionType p_actionType, unsigned p_maxPrepTime, unsigned p_maxExecTrialTime, unsigned p_maxExecTime)
: PlanStepEx(p_actionType, ESTATE_END), _preCondition(nullptr)
{
    _stateTimeout[INDEX(ESTATE_NotPrepared, ExecutionStateType)] = p_maxPrepTime;
    _stateTimeout[INDEX(ESTATE_Pending, ExecutionStateType)] = p_maxExecTime;
    _stateTimeout[INDEX(ESTATE_Executing, ExecutionStateType)] = p_maxExecTrialTime;
}
//////////////////////////////////////////////////////////////////////////
Action::Action(ActionType p_actionType, const PlanStepParameters& p_parameters, unsigned p_maxPrepTime,  unsigned p_maxExecTrialTime, unsigned p_maxExecTime)
: PlanStepEx(p_actionType, ESTATE_END, p_parameters), _preCondition(nullptr)
{
    _stateTimeout[INDEX(ESTATE_NotPrepared, ExecutionStateType)] = p_maxPrepTime;
    _stateTimeout[INDEX(ESTATE_Pending, ExecutionStateType)] = p_maxExecTime;
    _stateTimeout[INDEX(ESTATE_Executing, ExecutionStateType)] = p_maxExecTrialTime;
}
//////////////////////////////////////////////////////////////////////////
void Action::State(ExecutionStateType p_state, RtsGame* pRtsGame, const WorldClock& p_clock)
{
    PlanStepEx::State(p_state, pRtsGame, p_clock);

    switch (p_state)
    {
    case ESTATE_Succeeded:
        OnSucccess(pRtsGame, p_clock);
        break;
    case ESTATE_Failed:
        OnFailure(pRtsGame, p_clock);
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
void Action::InitializeConditions()
{
    PlanStepEx::InitializeConditions();
    InitializePreConditions();
}
//////////////////////////////////////////////////////////////////////////
bool Action::Execute(RtsGame* pRtsGame, const WorldClock& p_clock)
{
    bool bOk;

    assert(PlanStepEx::State() == ESTATE_Pending);
    bOk = ExecuteAux(pRtsGame, p_clock);

    return bOk;
}
//////////////////////////////////////////////////////////////////////////
void Action::Reset(RtsGame* pRtsGame, const WorldClock& p_clock)
{
    if (PlanStepEx::State() != ESTATE_NotPrepared)
        State(ESTATE_NotPrepared, pRtsGame, p_clock);
}
//////////////////////////////////////////////////////////////////////////
void Action::UpdateAux(RtsGame* pRtsGame, const WorldClock& p_clock)
{
    ExecutionStateType state = PlanStepEx::State();
    
    switch (state)
    {
    case ESTATE_NotPrepared:
        if (PreconditionsSatisfied(pRtsGame))
            State(ESTATE_Pending, pRtsGame, p_clock);
        break;

    case ESTATE_Pending:
        if (Execute(pRtsGame, p_clock))
            State(ESTATE_Executing, pRtsGame, p_clock);
        else
        {
            LogInfo("Executing '%s' failed", ToString().c_str());
            State(ESTATE_NotPrepared, pRtsGame, p_clock);
        }
        break;

    case ESTATE_Executing:
        if (SuccessConditionsSatisfied(pRtsGame))
            State(ESTATE_Succeeded, pRtsGame, p_clock);
        else if (!AliveConditionsSatisfied(pRtsGame))
        {
            LogInfo("%s alive conditions not satisfied, failing it", ToString().c_str());
            State(ESTATE_Failed, pRtsGame, p_clock);
        }
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
void Action::Copy(IClonable* p_dest)
{
    PlanStepEx::Copy(p_dest);

    Action* m_dest = static_cast<Action*>(p_dest);

    m_dest->_preCondition = _preCondition ? static_cast<CompositeExpression*>(_preCondition->Clone()) : nullptr;
    m_dest->_postCondition = _postCondition ? static_cast<CompositeExpression*>(_postCondition->Clone()) : nullptr;
}
