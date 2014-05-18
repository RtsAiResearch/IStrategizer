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

using namespace IStrategizer;
using namespace Serialization;
using namespace std;

EntityClassExist::EntityClassExist(PlayerType p_player, EntityClassType p_unitClassId, int p_amount, map< EntityObjectAttribute,int > p_entityObjectAttributes, int p_similarityThreshold) 
    : ConditionEx(p_player, CONDEX_EntityClassExist), _reusable(false)
{
    _conditionParameters[PARAM_EntityClassId] = p_unitClassId;
    _conditionParameters[PARAM_Amount] = p_amount;
    _similarityThreshold = p_similarityThreshold;
}
//---------------------------------------------------------------------------------------------------
EntityClassExist::EntityClassExist(PlayerType p_player, EntityClassType p_unitClassId, int p_amount, bool reusable) : ConditionEx(p_player, CONDEX_EntityClassExist), _reusable(false)
{
    _conditionParameters[PARAM_EntityClassId] = p_unitClassId;
    _conditionParameters[PARAM_Amount] = p_amount;
}
//---------------------------------------------------------------------------------------------------
EntityClassExist::EntityClassExist(PlayerType p_player, EntityClassType p_unitClassId, int p_amount) : ConditionEx(p_player, CONDEX_EntityClassExist), _reusable(false)
{
    _conditionParameters[PARAM_EntityClassId] = p_unitClassId;
    _conditionParameters[PARAM_Amount] = p_amount;
}
//---------------------------------------------------------------------------------------------------
EntityClassExist::EntityClassExist(PlayerType p_player, int p_amount) : ConditionEx(p_player, CONDEX_EntityClassExist), _reusable(false)
{
    _conditionParameters[PARAM_EntityClassId] = DONT_CARE;
    _conditionParameters[PARAM_Amount] = p_amount;
}
//---------------------------------------------------------------------------------------------------
EntityClassExist::EntityClassExist(PlayerType p_player) : ConditionEx(p_player, CONDEX_EntityClassExist), _reusable(false)
{
    _conditionParameters[PARAM_EntityClassId] = DONT_CARE;
    _conditionParameters[PARAM_Amount] = 0;
}
//---------------------------------------------------------------------------------------------------
bool EntityClassExist::Evaluate(RtsGame& game)
{
    if (_conditionParameters[PARAM_EntityClassId] != DONT_CARE)
    {
        EntityClassType entityClassId = (EntityClassType)_conditionParameters[PARAM_EntityClassId];
        int amount = _conditionParameters[PARAM_Amount];
        PlayerType playerId = (PlayerType)_conditionParameters[PARAM_PlayerId];

        ConditionEx::Evaluate(game);
        _isSatisfied = g_Assist.DoesEntityClassExist(MakePair(entityClassId, amount), playerId);
    }

    return _isEvaluated && _isSatisfied;
}
//---------------------------------------------------------------------------------------------------
void EntityClassExist::Copy(IClonable* p_dest)
{
    ConditionEx::Copy(p_dest);

    EntityClassExist* m_dest = static_cast<EntityClassExist*>(p_dest);
    m_dest->_similarityThreshold = _similarityThreshold;
}
//---------------------------------------------------------------------------------------------------
bool EntityClassExist::Consume(int p_amount)
{
    _ASSERTE(p_amount != DONT_CARE && _conditionParameters[PARAM_Amount] != DONT_CARE);
    if (_reusable && p_amount > 0)
    {
        // The entity is reusable so return true
        // There is no required amount to so return true.
        return true;
    }
    else if (p_amount == 0 || _conditionParameters[PARAM_Amount] == 0)
    {
        // The current available amount is 0 so can not consume anything.
        return false;
    }
    else
    {
        _ASSERTE(p_amount > 0 && _conditionParameters[PARAM_Amount] > 0);
        _conditionParameters[PARAM_Amount] -= p_amount;

        return true;
    }
}
