#include "ResourceExist.h"

#include "GamePlayer.h"
#include "PlayerResources.h"

using namespace IStrategizer;

ResourceExist::ResourceExist(PlayerType p_player, int p_resourceId, int p_amount) : ConditionEx(p_player, CONDEX_ResourceExist)
{
    _conditionParameters[PARAM_ResourceId] = p_resourceId;
    _conditionParameters[PARAM_Amount] = p_amount;
}
//---------------------------------------------------------------------------------------------------
bool ResourceExist::Evaluate(RtsGame& game)
{
    int requiredAmount = _conditionParameters[PARAM_Amount];
    g_Assist.GetResourceAmount((PlayerType)_conditionParameters[PARAM_PlayerId], (ResourceType)_conditionParameters[PARAM_ResourceId], _availableAmount);
    
    ConditionEx::Evaluate(game);
    _isSatisfied = _isEvaluated && (_availableAmount >= requiredAmount);

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
    if (p_amount == 0 || _conditionParameters[PARAM_Amount] == 0)
    {
        // The required amount is 0, return false as there's nothing to consume
        // The current available amount is 0 so can not consume anything.
        return false;
    }
    else if (_conditionParameters[PARAM_Amount] == DONT_CARE)
    {
        // I am infinite source so always return true.
        return true;
    }
    else if (p_amount == DONT_CARE)
    {
        // DONT_CARE amount means infinite required amount, so return true and consume all amount the condition has.
        _conditionParameters[PARAM_Amount] = 0;
        return true;
    }
    else
    {
        _ASSERTE(p_amount > 0 && _conditionParameters[PARAM_Amount] > 0);
        // Consider consuming supply resources only as for primary and secondary
        // we don't use exact numbers.
        if (_conditionParameters[PARAM_ResourceId] == RESOURCE_Supply)
        {
            _conditionParameters[PARAM_Amount] -= p_amount;
        }

        return true;
    }
}
//----------------------------------------------------------------------------------------------
