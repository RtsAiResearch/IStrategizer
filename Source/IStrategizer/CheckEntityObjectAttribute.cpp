#include "CheckEntityObjectAttribute.h"

CheckEntityObjectAttribute::CheckEntityObjectAttribute(PlayerType p_player, int p_entityObjectId, int p_attributeId, int p_operator, int p_value)
: ConditionEx(p_player, CONDEX_CheckEntityObjectAttribute)
{
	_conditionParameters[PARAM_EntityObjectId]	= p_entityObjectId;
	_conditionParameters[PARAM_AttributeId]		= p_attributeId;
	_conditionParameters[PARAM_OperatorId]		= p_operator;
	_conditionParameters[PARAM_Value]			= p_value;
}
//----------------------------------------------------------------------------------------------
CheckEntityObjectAttribute::CheckEntityObjectAttribute(PlayerType p_player, int* p_entityObjectIdPtr, int p_attributeId, int p_operator, int p_value)
: ConditionEx(p_player, CONDEX_CheckEntityObjectAttribute)
{
	_entityObjectIdPtr = p_entityObjectIdPtr;
	_conditionParameters[PARAM_AttributeId] = p_attributeId;
	_conditionParameters[PARAM_OperatorId]	= p_operator;
	_conditionParameters[PARAM_Value]		= p_value;
}
//---------------------------------------------------------------------------------------------------
bool CheckEntityObjectAttribute::Evaluate()
{
	if(_conditionParameters.find(PARAM_EntityObjectId) == _conditionParameters.end())
	{
		assert(_entityObjectIdPtr != NULL);
		_conditionParameters[PARAM_EntityObjectId] = *_entityObjectIdPtr;
	}

	int m_returnValue = g_Assist.GetEntityObjectAttribute((PlayerType)_conditionParameters[PARAM_PlayerId], _conditionParameters[PARAM_EntityObjectId], 
		(EntityObjectAttribute)_conditionParameters[PARAM_AttributeId], _currentValue);

	ConditionEx::Evaluate();

	if (m_returnValue == ERR_Success)
	{
		if (Comparer::Compare(_currentValue,  (RelationalOperatorType)_conditionParameters[PARAM_OperatorId], _conditionParameters[PARAM_Value]))
		{
			_isSatisfied = true;
		}
		else
		{
			_isSatisfied = false;
		}

		_isEvaluated = true;
	}

	return _isEvaluated && _isSatisfied;
}
//----------------------------------------------------------------------------------------------
void CheckEntityObjectAttribute::Copy(IClonable* p_dest)
{
    ConditionEx::Copy(p_dest);

    CheckEntityObjectAttribute* m_dest =static_cast<CheckEntityObjectAttribute*>(p_dest);

    m_dest->_currentValue = _currentValue;
}
