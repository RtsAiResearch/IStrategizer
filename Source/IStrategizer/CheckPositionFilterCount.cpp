#include "CheckPositionFilterCount.h"


CheckPositionFilterCount::CheckPositionFilterCount(PlayerType p_player, int p_filterTypeId, int p_operatorId, int p_value, const Vector2& p_cellIndex)
: ConditionEx(p_player, CONDEX_CheckPositionFilterCount)
{
    _conditionParameters[PARAM_FilterTypeId] = p_filterTypeId;
    _conditionParameters[PARAM_OperatorId] = p_operatorId;
    _conditionParameters[PARAM_Value] = p_value;
	_cellIndex = p_cellIndex;
}

//---------------------------------------------------------------------------------------------------
bool CheckPositionFilterCount::Evaluate()
{
    int m_returnValue;

    ConditionEx::Evaluate();

	//FIXME : Vector2:Null() is wrong , original was PositionFeatureVector::From(_conditionParameters)
    m_returnValue = g_Assist.GetFilterCount(_playerType, (FilterType)_conditionParameters[PARAM_FilterTypeId], 
		Vector2::Null(), _currentCount);

    if (m_returnValue == ERR_Success)
    {
        if (Comparer::Compare(_currentCount,  (RelationalOperatorType)_conditionParameters[PARAM_OperatorId], _conditionParameters[PARAM_Value]))
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
//---------------------------------------------------------------------------------------------------
void CheckPositionFilterCount::Copy(IClonable* p_dest)
{
    ConditionEx::Copy(p_dest);

	CheckPositionFilterCount* m_dest = static_cast<CheckPositionFilterCount*>(p_dest);

	m_dest->_currentCount = _currentCount;
}
