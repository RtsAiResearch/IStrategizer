#include "Action.h"
#include <cassert>
#ifndef AND_H
#include "And.h"
#endif

Action::Action(ActionType p_actionType, unsigned p_maxPrepTime, unsigned p_maxExecTrialTime, unsigned p_maxExecTime)
: PlanStepEx(p_actionType, ESTATE_END), _preCondition(NULL), _aliveCondition(NULL)
{
	_stateTimeout[INDEX(ESTATE_NotPrepared, ExecutionStateType)] = p_maxPrepTime;
	_stateTimeout[INDEX(ESTATE_Pending, ExecutionStateType)] = p_maxExecTime;
	_stateTimeout[INDEX(ESTATE_Executing, ExecutionStateType)] = p_maxExecTrialTime;
}
//////////////////////////////////////////////////////////////////////////
Action::Action(ActionType p_actionType, const PlanStepParameters& p_parameters, unsigned p_maxPrepTime,  unsigned p_maxExecTrialTime, unsigned p_maxExecTime)
: PlanStepEx(p_actionType, ESTATE_END, p_parameters), _preCondition(NULL), _aliveCondition(NULL)
{
	_stateTimeout[INDEX(ESTATE_NotPrepared, ExecutionStateType)] = p_maxPrepTime;
	_stateTimeout[INDEX(ESTATE_Pending, ExecutionStateType)] = p_maxExecTime;
	_stateTimeout[INDEX(ESTATE_Executing, ExecutionStateType)] = p_maxExecTrialTime;
}
//////////////////////////////////////////////////////////////////////////
void Action::InitializeConditions()
{
	InitializePreConditions();
	InitializePostConditions();
}
//////////////////////////////////////////////////////////////////////////
int Action::PrepareForExecution(unsigned p_cyles)
{
	assert(0);
	//assert(State() == ESTATE_NotPrepared);

	//if(_prepTime == 0)
	//{
	//	_prepTime = p_cyles;
	//}
	//else if(p_cyles - _prepTime > _maxPrepTime)
	//{
	//	_prepTime = 0;
	//	State(ESTATE_Failed);
	//}

	//if(PreconditionsSatisfied())
	//{
	//	// Reset the prep start time in case the action failed while execution and we try to prepare it again
	//	_prepTime = 0;
	//	State(ESTATE_Pending);
	//}

	return ERR_Success;
}
//////////////////////////////////////////////////////////////////////////
int Action::Execute(unsigned p_cycles)
{
	int m_returnValue = ERR_Success;

	assert(State() == ESTATE_Pending);
	m_returnValue = ExecuteAux(p_cycles);

	return m_returnValue;
}
//////////////////////////////////////////////////////////////////////////
void Action::Reset(unsigned p_cycles)
{
	if (State() != ESTATE_NotPrepared)
		State(ESTATE_NotPrepared, p_cycles);
}
//////////////////////////////////////////////////////////////////////////
void Action::UpdateAux(unsigned p_cycles)
{
	ExecutionStateType state = State();
	
	switch (state)
	{
	case ESTATE_NotPrepared:
		if (PreconditionsSatisfied())
			State(ESTATE_Pending, p_cycles);
		break;

	case ESTATE_Pending:
		if (Execute(p_cycles) == ERR_Success)
			State(ESTATE_Executing, p_cycles);
		else
			printf("Planner: Executing '%s' failed\n", ToString());
		break;

	case ESTATE_Executing:
		if (SuccessConditionsSatisfied())
			State(ESTATE_Succeeded, p_cycles);
		else if (!AliveConditionsSatisfied())
			State(ESTATE_Failed, p_cycles);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
void Action::Copy(IClonable* p_dest)
{
	PlanStepEx::Copy(p_dest);

	Action* m_dest = static_cast<Action*>(p_dest);

	m_dest->_preCondition	= _preCondition ? static_cast<CompositeExpression*>(_preCondition->Clone()) : NULL;
	m_dest->_aliveCondition	= _aliveCondition ? static_cast<CompositeExpression*>(_aliveCondition->Clone()) : NULL;
}
