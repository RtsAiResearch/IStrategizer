#include "ResourceExist.h"

ResourceExist::ResourceExist(PlayerType p_player, int p_resourceId, int p_amount) : ConditionEx(p_player, CONDEX_ResourceExist)
{
	_conditionParameters[PARAM_ResourceId] = p_resourceId;
	_conditionParameters[PARAM_Amount] = p_amount;
}
//---------------------------------------------------------------------------------------------------
bool ResourceExist::Evaluate()
{
	int returnValue = g_Assist.GetResourceAmount((PlayerType)_conditionParameters[PARAM_PlayerId], (ResourceType)_conditionParameters[PARAM_ResourceId], _availableAmount);

	ConditionEx::Evaluate();

	_isEvaluated = (returnValue == ERR_Success);
	_isSatisfied = _isEvaluated && (_availableAmount >= _conditionParameters[PARAM_Amount]);

	return _isEvaluated && _isSatisfied;
}
//----------------------------------------------------------------------------------------------
void ResourceExist::Copy(IClonable* p_dest)
{
    ConditionEx::Copy(p_dest);

    ResourceExist* m_dest = static_cast<ResourceExist*>(p_dest);

    m_dest->_availableAmount = _availableAmount;
}
//----------------------------------------------------------------------------------------------
bool ResourceExist::Consume(int p_amount)
{
	if (_conditionParameters[PARAM_Amount] >= p_amount)
	{
		_conditionParameters[PARAM_Amount] -= p_amount;
		return true;
	}

	return false;
}
//----------------------------------------------------------------------------------------------
