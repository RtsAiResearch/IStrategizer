#include "EntityClassExist.h"
#ifndef GAMEPLAYER_H
#include "GamePlayer.h"
#endif
#ifndef GAMEENTITY_H
#include "GameEntity.h"
#endif
#ifndef RTSGAME_H
#include "RtsGame.h"
#endif
#include "ObjectFactory.h"

using namespace IStrategizer;
using namespace Serialization;
using namespace std;

DECL_SERIALIZABLE(EntityClassExist);

EntityClassExist::EntityClassExist(PlayerType p_player, EntityClassType p_unitClassId, int p_amount, ObjectStateType state, bool checkFree) :
ConditionEx(p_player, CONDEX_EntityClassExist),
m_checkFree(checkFree)
{
    m_params[PARAM_EntityClassId] = p_unitClassId;
    m_params[PARAM_Amount] = p_amount;
    m_params[PARAM_ObjectStateType] = state;
}
//---------------------------------------------------------------------------------------------------
EntityClassExist::EntityClassExist(PlayerType p_player, EntityClassType p_unitClassId, ObjectStateType state, bool checkFree) :
ConditionEx(p_player, CONDEX_EntityClassExist),
m_checkFree(checkFree)
{
    m_params[PARAM_EntityClassId] = p_unitClassId;
    m_params[PARAM_Amount] = DONT_CARE;
    m_params[PARAM_ObjectStateType] = state;
}
//---------------------------------------------------------------------------------------------------
EntityClassExist::EntityClassExist(PlayerType p_player) :
ConditionEx(p_player, CONDEX_EntityClassExist),
m_checkFree(false)
{
    m_params[PARAM_EntityClassId] = OBJSTATE_END;
    m_params[PARAM_Amount] = DONT_CARE;
}
//---------------------------------------------------------------------------------------------------
bool EntityClassExist::Evaluate(RtsGame& game)
{
    if (m_params[PARAM_EntityClassId] != OBJSTATE_END)
    {
        EntityClassType entityClassId = (EntityClassType)m_params[PARAM_EntityClassId];
        int amount = m_params[PARAM_Amount];
        auto state = (ObjectStateType)m_params[PARAM_ObjectStateType];
        PlayerType playerId = (PlayerType)m_params[PARAM_PlayerId];

        ConditionEx::Evaluate(game);
        _isSatisfied = g_Assist.DoesEntityClassExist(make_pair(entityClassId, amount), state, m_checkFree, playerId);
    }

    return _isEvaluated && _isSatisfied;
}
//---------------------------------------------------------------------------------------------------
bool EntityClassExist::Consume(int p_amount)
{
    if (p_amount > 0 || m_params[PARAM_Amount] == DONT_CARE)
    {
        // (1) The entity is reusable so return true
        // There is no required amount to so return true.
        // (2) The source is infinite so always return true
        return true;
    }
    else if (p_amount == 0 || m_params[PARAM_Amount] == 0)
    {
        // The current available amount is 0 so can not consume anything.
        return false;
    }
    else if (p_amount == DONT_CARE && m_params[PARAM_Amount] != DONT_CARE)
    {
        // The request is infinite source, consume all available amount
        _ASSERTE(m_params[PARAM_Amount] > 0);
        m_params[PARAM_Amount] = 0;

        return true;
    }
    else
    {
        _ASSERTE(p_amount > 0 && m_params[PARAM_Amount] > 0);
        m_params[PARAM_Amount] -= p_amount;

        return true;
    }
}
