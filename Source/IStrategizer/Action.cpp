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
void Action::State(ExecutionStateType p_state, RtsGame& p_RtsGame, const WorldClock& p_clock)
{
    PlanStepEx::State(p_state, p_RtsGame, p_clock);

    switch (p_state)
    {
    case ESTATE_Succeeded:
        OnSucccess(p_RtsGame, p_clock);
        break;
    case ESTATE_Failed:
        OnFailure(p_RtsGame, p_clock);
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
void Action::InitializeConditions(RtsGame& p_RtsGame)
{
    PlanStepEx::InitializeConditions();
    InitializePreConditions(p_RtsGame);
}
//////////////////////////////////////////////////////////////////////////
bool Action::Execute(RtsGame& p_RtsGame, const WorldClock& p_clock)
{
    bool bOk;

    assert(PlanStepEx::State() == ESTATE_Pending);
    bOk = ExecuteAux(p_RtsGame, p_clock);

    return bOk;
}
//////////////////////////////////////////////////////////////////////////
void Action::Reset(RtsGame& p_RtsGame, const WorldClock& p_clock)
{
    if (PlanStepEx::State() != ESTATE_NotPrepared)
        State(ESTATE_NotPrepared, p_RtsGame, p_clock);
}
//////////////////////////////////////////////////////////////////////////
void Action::UpdateAux(RtsGame& p_RtsGame, const WorldClock& p_clock)
{
    ExecutionStateType state = PlanStepEx::State();
    
    switch (state)
    {
    case ESTATE_NotPrepared:
        if (PreconditionsSatisfied(p_RtsGame))
            State(ESTATE_Pending, p_RtsGame, p_clock);
        break;

    case ESTATE_Pending:
        if (Execute(p_RtsGame, p_clock))
            State(ESTATE_Executing, p_RtsGame, p_clock);
        else
        {
            LogInfo("Executing '%s' failed", ToString().c_str());
            State(ESTATE_NotPrepared, p_RtsGame, p_clock);
        }
        break;

    case ESTATE_Executing:
        if (SuccessConditionsSatisfied(p_RtsGame))
            State(ESTATE_Succeeded, p_RtsGame, p_clock);
        else if (!AliveConditionsSatisfied(p_RtsGame))
        {
            LogInfo("%s alive conditions not satisfied, failing it", ToString().c_str());
            State(ESTATE_Failed, p_RtsGame, p_clock);
        }
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
bool Action::PreconditionsSatisfied(RtsGame& p_RtsGame)
{
    if (_preCondition == nullptr)
        InitializeConditions(p_RtsGame);

    return _preCondition->Evaluate(p_RtsGame);
}
//////////////////////////////////////////////////////////////////////////
void Action::Copy(IClonable* p_dest)
{
    PlanStepEx::Copy(p_dest);

    Action* m_dest = static_cast<Action*>(p_dest);

    m_dest->_preCondition = _preCondition ? static_cast<CompositeExpression*>(_preCondition->Clone()) : nullptr;
    m_dest->_postCondition = _postCondition ? static_cast<CompositeExpression*>(_postCondition->Clone()) : nullptr;
}
