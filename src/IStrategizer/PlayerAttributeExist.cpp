#include "PlayerAttributeExist.h"
#include "EngineData.h"
#include "GamePlayer.h"

using namespace IStrategizer;

PlayerAttributeExist::PlayerAttributeExist(PlayerType playerType, PlayerAttribute p_attribute, int amount) : 
    m_attribute(p_attribute),
    ConditionEx(playerType, CONDEX_PlayerAttributeExist)
{
    switch (p_attribute)
    {
    case PATTR_AlliedUnitsTotalHP:
        _conditionParameters[PARAM_AlliedUnitsTotalHP] = 1;
        break;

    case PATTR_AlliedUnitsTotalDamage:
        _conditionParameters[PARAM_AlliedUnitsTotalDamage] = 1;
        break;

    default:
        _ASSERT(!"Invalid player attribute!");
        break;
    }

    _conditionParameters[PARAM_Amount] = amount;
}
//////////////////////////////////////////////////////////////////////////
bool PlayerAttributeExist::Consume(int p_amount)
{
    if (p_amount == 0 || _conditionParameters[PARAM_Amount] == 0)
    {
        return false;
    }
    else if (p_amount > _conditionParameters[PARAM_Amount])
    {
        return false;
    }
    else
    {
        _conditionParameters[PARAM_Amount] -= p_amount;
        return true;
    }
}
//////////////////////////////////////////////////////////////////////////
bool PlayerAttributeExist::Evaluate(RtsGame& game)
{
    if (_playerType == PLAYER_Self)
    {
        return game.Self()->Attr(m_attribute) > _conditionParameters[PARAM_Amount];
    }
    else if (_playerType == PLAYER_Enemy)
    {
        return game.Enemy()->Attr(m_attribute) > _conditionParameters[PARAM_Amount];
    }
    else
    {
        _ASSERTE(!"Invalid player type!");
        return false;
    }
}