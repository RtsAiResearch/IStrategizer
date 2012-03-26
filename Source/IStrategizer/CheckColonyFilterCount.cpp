#include "CheckColonyFilterCount.h"
#include "Colony.h"

CheckColonyFilterCount::CheckColonyFilterCount(PlayerType p_player, int p_filterTypeId, int p_operatorId, int p_value, const Colony& p_colony)
: ConditionEx(p_player, CONDEX_CheckColonyFilterCount)
{
    _conditionParameters[PARAM_FilterTypeId] = p_filterTypeId;
    _conditionParameters[PARAM_OperatorId] = p_operatorId;
    _conditionParameters[PARAM_Value] = p_value;

    _colony = p_colony;
}
//----------------------------------------------------------------------------------------------
void CheckColonyFilterCount::InitializeAddressesAux()
{
    ConditionEx::InitializeAddressesAux();
    AddMemberAddress(1,
        &_colony);
}
//---------------------------------------------------------------------------------------------------
bool CheckColonyFilterCount::Evaluate()
{
    int m_returnValue;

    ConditionEx::Evaluate();

     m_returnValue = g_Assist.GetFilterCount(_playerType, (FilterType)_conditionParameters[PARAM_FilterTypeId], 
            _colony, _currentCount);

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
//----------------------------------------------------------------------------------------------
void CheckColonyFilterCount::Copy(IClonable* p_dest)
{
    ConditionEx::Copy(p_dest);

    CheckColonyFilterCount* m_dest = static_cast<CheckColonyFilterCount*>(p_dest);

    m_dest->_currentCount = _currentCount;
    _colony.Copy(&m_dest->_colony);
}
