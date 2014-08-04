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
    case PATTR_AlliedAttackersTotalHP:
        m_params[PARAM_AlliedAttackersTotalHP] = 1;
        break;

    case PATTR_AlliedAttackersTotalDamage:
        m_params[PARAM_AlliedAttackersTotalDamage] = 1;
        break;

    default:
        _ASSERT(!"Invalid player attribute!");
        break;
    }

    m_params[PARAM_Amount] = amount;
}
//////////////////////////////////////////////////////////////////////////
bool PlayerAttributeExist::Consume(int p_amount)
{
    if (p_amount == 0 || m_params[PARAM_Amount] == 0)
    {
        return false;
    }
    else if (p_amount > m_params[PARAM_Amount])
    {
        return false;
    }
    else
    {
        m_params[PARAM_Amount] -= p_amount;
        return true;
    }
}
//////////////////////////////////////////////////////////////////////////
bool PlayerAttributeExist::Evaluate(RtsGame& game)
{
    if (_playerType == PLAYER_Self)
    {
        return game.Self()->Attr(m_attribute) > m_params[PARAM_Amount];
    }
    else if (_playerType == PLAYER_Enemy)
    {
        return game.Enemy()->Attr(m_attribute) > m_params[PARAM_Amount];
    }
    else
    {
        _ASSERTE(!"Invalid player type!");
        return false;
    }
}