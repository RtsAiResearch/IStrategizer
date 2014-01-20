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
bool ResourceExist::Evaluate(RtsGame& pRtsGame)
{
    EngineAssist::Instance(g_Game).GetResourceAmount((PlayerType)_conditionParameters[PARAM_PlayerId], (ResourceType)_conditionParameters[PARAM_ResourceId], _availableAmount);
    
    ConditionEx::Evaluate(pRtsGame);
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
