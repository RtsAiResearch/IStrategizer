#include "Action.h"
#include <cassert>
#ifndef AND_H
#include "And.h"
#endif
#include "Logger.h"
#include "CompositeExpression.h"

using namespace IStrategizer;

Action::Action(ActionType p_actionType, unsigned p_maxPrepTime, unsigned p_maxExecTrialTime, unsigned p_maxExecTime)
: PlanStepEx(p_actionType, ESTATE_END), _preCondition(nullptr), _aliveCondition(nullptr)
{
    _stateTimeout[INDEX(ESTATE_NotPrepared, ExecutionStateType)] = p_maxPrepTime;
    _stateTimeout[INDEX(ESTATE_Pending, ExecutionStateType)] = p_maxExecTime;
    _stateTimeout[INDEX(ESTATE_Executing, ExecutionStateType)] = p_maxExecTrialTime;
}
//////////////////////////////////////////////////////////////////////////
Action::Action(ActionType p_actionType, const PlanStepParameters& p_parameters, unsigned p_maxPrepTime,  unsigned p_maxExecTrialTime, unsigned p_maxExecTime)
: PlanStepEx(p_actionType, ESTATE_END, p_parameters), _preCondition(nullptr), _aliveCondition(nullptr)
{
    _stateTimeout[INDEX(ESTATE_NotPrepared, ExecutionStateType)] = p_maxPrepTime;
    _stateTimeout[INDEX(ESTATE_Pending, ExecutionStateType)] = p_maxExecTime;
    _stateTimeout[INDEX(ESTATE_Executing, ExecutionStateType)] = p_maxExecTrialTime;
}
//////////////////////////////////////////////////////////////////////////
void Action::State(ExecutionStateType p_state, const WorldClock& p_clock)
{
    PlanStepEx::State(p_state, p_clock);

    switch (p_state)
    {
    case ESTATE_Succeeded:
        OnSucccess(p_clock);
        break;
    case ESTATE_Failed:
        OnFailure(p_clock);
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
void Action::InitializeConditions()
{
    InitializePreConditions();
    InitializePostConditions();
}
//////////////////////////////////////////////////////////////////////////
int Action::PrepareForExecution(const WorldClock& p_clock)
{
    assert(0);
    //assert(State() == ESTATE_NotPrepared);

    //if(_prepTime == 0)
    //{
    // _prepTime = p_cyles;
    //}
    //else if(p_cyles - _prepTime > _maxPrepTime)
    //{
    // _prepTime = 0;
    // State(ESTATE_Failed);
    //}

    //if(PreconditionsSatisfied())
    //{
    // // Reset the prep start time in case the action failed while execution and we try to prepare it again
    // _prepTime = 0;
    // State(ESTATE_Pending);
    //}

    return ERR_Success;
}
//////////////////////////////////////////////////////////////////////////
bool Action::Execute(const WorldClock& p_clock)
{
    bool bOk;

    assert(PlanStepEx::State() == ESTATE_Pending);
    bOk = ExecuteAux(p_clock);

    return bOk;
}
//////////////////////////////////////////////////////////////////////////
void Action::Reset(const WorldClock& p_clock)
{
    if (PlanStepEx::State() != ESTATE_NotPrepared)
        State(ESTATE_NotPrepared, p_clock);
}
//////////////////////////////////////////////////////////////////////////
void Action::UpdateAux(const WorldClock& p_clock)
{
    ExecutionStateType state = PlanStepEx::State();
    
    switch (state)
    {
    case ESTATE_NotPrepared:
        if (PreconditionsSatisfied())
            State(ESTATE_Pending, p_clock);
        break;

    case ESTATE_Pending:
        if (Execute(p_clock))
            State(ESTATE_Executing, p_clock);
        else
            LogInfo("Executing '%s' failed", ToString().c_str());
        break;

    case ESTATE_Executing:
        if (SuccessConditionsSatisfied())
            State(ESTATE_Succeeded, p_clock);
        else if (!AliveConditionsSatisfied())
        {
            LogInfo("%s alive conditions not satisfied, failing it", ToString().c_str());
            State(ESTATE_Failed, p_clock);
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
    m_dest->_aliveCondition = _aliveCondition ? static_cast<CompositeExpression*>(_aliveCondition->Clone()) : nullptr;
}
