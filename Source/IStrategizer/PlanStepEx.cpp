#ifndef PLANSTEPEX_H
#include "PlanStepEx.h"
#endif

#include <cstdio>
#include <algorithm>

using namespace std;

//////////////////////////////////////////////////////////////////////////
void PlanStepEx::InitializeAddressesAux()
{
	AddMemberAddress(1,
		&_params);
}
//////////////////////////////////////////////////////////////////////////
PlanStepEx::PlanStepEx(int p_stepTypeId, ExecutionStateType p_state) :
_stepTypeId(p_stepTypeId), _state(p_state), _successCondition(NULL), _postCondition(NULL), _firstUpdate(true)
{
	memset(_stateStartTime, 0, sizeof(_stateStartTime));
	memset(_stateTimeout, 0, sizeof(_stateTimeout));
}
//////////////////////////////////////////////////////////////////////////
PlanStepEx::PlanStepEx(int p_stepTypeId, ExecutionStateType p_state, const PlanStepParameters& p_parameters) : 
_stepTypeId(p_stepTypeId), _state(p_state), _params(p_parameters), _successCondition(NULL), _postCondition(NULL), _firstUpdate(true)
{
	memset(_stateStartTime, 0, sizeof(_stateStartTime));
	memset(_stateTimeout, 0, sizeof(_stateTimeout));
}
//////////////////////////////////////////////////////////////////////////
bool PlanStepEx::Equals(PlanStepEx* p_planStep)
{
	bool m_equals = _stepTypeId == p_planStep->_stepTypeId;

	for (PlanStepParameters::const_iterator i = _params.begin(), j = p_planStep->_params.begin();
		m_equals && i != _params.end(); ++i, ++j)
	{
		m_equals &= ((*i).first == (*j).first) && ((*i).second == (*j).second);
	}

	return m_equals;
}
//////////////////////////////////////////////////////////////////////////
void PlanStepEx::InitializeConditions()
{
	InitializeSuccessConditions();
	InitializePostConditions();
}
//////////////////////////////////////////////////////////////////////////
IClonable* PlanStepEx::Clone()
{
    PlanStepEx* clone = static_cast<PlanStepEx*>(Prototype());
    Copy(clone);

    return clone;
}
//////////////////////////////////////////////////////////////////////////
void PlanStepEx::Copy(IClonable* p_dest)
{
    PlanStepEx* m_dest = static_cast<PlanStepEx*>(p_dest);

	m_dest->_stepTypeId         = _stepTypeId;
	m_dest->_state              = _state;
	m_dest->_params         = _params;
    m_dest->_successCondition   = _successCondition ? static_cast<CompositeExpression*>(_successCondition->Clone()) : NULL;
    m_dest->_postCondition      = _postCondition ?    static_cast<CompositeExpression*>(_postCondition->Clone()) : NULL;
	m_dest->_stepLevelType      = _stepLevelType;
    m_dest->_data               = _data;
}
//////////////////////////////////////////////////////////////////////////
void PlanStepEx::State(ExecutionStateType p_state, unsigned p_cycles)
{
	printf("%s: '%s'->'%s'\n", ToString(), Enums[_state], Enums[p_state]);
	_stateStartTime[INDEX(p_state, ExecutionStateType)] = p_cycles;
	_state = p_state;
}
//////////////////////////////////////////////////////////////////////////
bool PlanStepEx::IsCurrentStateTimeout(unsigned p_cycles)
{
	unsigned timeout = _stateTimeout[INDEX(_state, ExecutionStateType)];
	unsigned startTime = _stateStartTime[INDEX(_state, ExecutionStateType)];

	// 0 means no timeout and thus meaning infinite timeout
	if (timeout == 0)
		return false;
	else
		return ((p_cycles - startTime) > timeout);
}
//////////////////////////////////////////////////////////////////////////
void PlanStepEx::Update(unsigned p_cycles)
{
	if (_firstUpdate)
	{
		Reset(p_cycles);
		_firstUpdate = false;
	}

	if (IsCurrentStateTimeout(p_cycles))
		State(ESTATE_Failed, p_cycles);
	else
	{
		UpdateAux(p_cycles);
	}
}
//////////////////////////////////////////////////////////////////////////
const char* PlanStepEx::ToString()
{
	if (_stepDescription.empty())
	{
		const char* stepName = Enums[_stepTypeId];
		unsigned	paramIdx = 0;

		_stepDescription = '(';
		_stepDescription += stepName;
		_stepDescription += ')';
		_stepDescription += '<';

		for (PlanStepParameters::iterator itr = _params.begin();
			itr != _params.end(); ++itr)
		{
			_stepDescription += Enums[itr->second];

			if (paramIdx < _params.size() - 1)
			{
				_stepDescription += ',';
				_stepDescription += ' ';
			}

			++paramIdx;
		}

		_stepDescription += '>';
	}
	
	return _stepDescription.c_str();
}