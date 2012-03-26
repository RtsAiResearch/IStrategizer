#include "CheckEntityClassAttribute.h"
#include "RtsGame.h"
#include <cassert>

CheckEntityClassAttribute::CheckEntityClassAttribute(PlayerType p_player, int p_entityClssId, int p_attributeId, int p_operator, int p_value)
: ConditionEx(p_player, CONDEX_CheckEntityClassAttribute)
{
	_conditionParameters[PARAM_EntityClassId] = p_entityClssId;
	_conditionParameters[PARAM_AttributeId] = p_attributeId;
	_conditionParameters[PARAM_OperatorId] = p_operator;
	_conditionParameters[PARAM_Value] = p_value;
}
//---------------------------------------------------------------------------------------------------
bool CheckEntityClassAttribute::Evaluate()
{
	switch (_conditionParameters[PARAM_EntityClassId])
	{
	case PARAM_END:
		CheckForAllEntities();
		break;
	default:
		CheckForSingleEntity((EntityClassType)_conditionParameters[PARAM_EntityClassId]);
		break;
	}

	return _isEvaluated && _isSatisfied;
}
//---------------------------------------------------------------------------------------------------
void CheckEntityClassAttribute::CheckForSingleEntity(EntityClassType p_entityClassType)
{
	int m_returnValue = g_Assist.GetEntityClassAttribute((PlayerType)_conditionParameters[PARAM_PlayerId], p_entityClassType, 
		(EntityClassAttribute)_conditionParameters[PARAM_AttributeId], _currentValue);

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
}
//---------------------------------------------------------------------------------------------------
void CheckEntityClassAttribute::CheckForAllEntities()
{
	vector<EntityClassType> entityTypes;

	g_Game->EntityTypes(entityTypes);

	_isEvaluated = _isSatisfied = true;
	for (int i = 0, size = entityTypes.size(); i < size && _isEvaluated && _isSatisfied; ++i)
	{
		CheckForSingleEntity((EntityClassType)entityTypes[i]);
	}
}
//---------------------------------------------------------------------------------------------------
void CheckEntityClassAttribute::Copy(IClonable* p_dest)
{
    ConditionEx::Copy(p_dest);

	CheckEntityClassAttribute* m_dest = static_cast<CheckEntityClassAttribute*>(p_dest);

	m_dest->_currentValue = _currentValue;
}