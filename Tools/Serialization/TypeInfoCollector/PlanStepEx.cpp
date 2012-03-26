#ifndef PLANSTEPEX_H
	#include "PlanStepEx.h"
#endif

//----------------------------------------------------------------------------------------------
void PlanStepEx::InitializeAddressesAux()
{
    AddMemberAddress(7,
        &_stepTypeId,
        &_parameters,
        &_isGoal,
        &_aliveCondition,
        &_successCondition,
        &_postCondition,
        &_stepLevelType);
}
//----------------------------------------------------------------------------------------------
PlanStepEx::PlanStepEx(int p_stepTypeId, ExecutionStateType p_state, const PlanStepParameters& p_parameters, StepLevelType p_stepLevelType) : 
					   _stepTypeId(p_stepTypeId), _isGoal(BELONG(GoalTypeEx, p_stepTypeId)), _state(p_state), _parameters(p_parameters), _aliveCondition(NULL), 
					   _successCondition(NULL), _postCondition(NULL), _stepLevelType(p_stepLevelType)
{
	if (_parameters[PARAM_ForceSizeId])
	{
		switch (_parameters[PARAM_ForceSizeId])
		{
		case FORCESIZE_UnaryForce:		_parameters[PARAM_Value] = 1; break;
		case FORCESIZE_TinyForce:		_parameters[PARAM_Value] = 2; break;
		case FORCESIZE_SmallForce:		_parameters[PARAM_Value] = 5; break;
		case FORCESIZE_ModerateForce:	_parameters[PARAM_Value] = 7; break;
		case FORCESIZE_BigForce:		_parameters[PARAM_Value] = 10; break;
		case FORCESIZE_LargeForce:		_parameters[PARAM_Value] = 15; break;
		case FORCESIZE_HugeForce:		_parameters[PARAM_Value] = 20; break;
		case FORCESIZE_GiganticForce:	_parameters[PARAM_Value] = 25; break;
		}
	}
}
//-------------------------------------------------------------------------------------------------------
bool PlanStepEx::Equals(PlanStepEx* p_planStep)
{
	bool m_equals = _stepTypeId == p_planStep->_stepTypeId;

	for (PlanStepParameters::const_iterator i = _parameters.begin(), j = p_planStep->_parameters.begin();
		m_equals && i != _parameters.end(); ++i, ++j)
	{
		m_equals &= ((*i).first == (*j).first) && ((*i).second == (*j).second);
	}

	return m_equals;
}
//-------------------------------------------------------------------------------------------------------
void PlanStepEx::Initialize(PlanStepEx* p_destination)
{
	p_destination->_stepTypeId = _stepTypeId;
	p_destination->_state = _state;
	p_destination->_parameters = _parameters;
	p_destination->_isGoal = _isGoal;
	p_destination->_aliveCondition = _aliveCondition->Clone();
	p_destination->_successCondition = _successCondition->Clone();
	p_destination->_postCondition = _postCondition->Clone();
	p_destination->_stepLevelType = _stepLevelType;
}