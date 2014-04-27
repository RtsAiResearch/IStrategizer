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
    : ConditionEx(p_player, CONDEX_EntityClassExist)
{
    _conditionParameters[PARAM_EntityClassId] = p_unitClassId;
    _conditionParameters[PARAM_Amount] = p_amount;
    _similarityThreshold = p_similarityThreshold;

    _oneUse = false;
    _used = false;
}
//---------------------------------------------------------------------------------------------------
EntityClassExist::EntityClassExist(PlayerType p_player, EntityClassType p_unitClassId, int p_amount, bool p_oneUse) : ConditionEx(p_player, CONDEX_EntityClassExist)
{
    _conditionParameters[PARAM_EntityClassId] = p_unitClassId;
    _conditionParameters[PARAM_Amount] = p_amount;

    _oneUse = p_oneUse;
    _used = false;
}
//---------------------------------------------------------------------------------------------------
EntityClassExist::EntityClassExist(PlayerType p_player, int p_amount, bool p_oneUse) : ConditionEx(p_player, CONDEX_EntityClassExist)
{
    _conditionParameters[PARAM_EntityClassId] = DONT_CARE;
    _conditionParameters[PARAM_Amount] = p_amount;

    _oneUse = p_oneUse;
    _used = false;
}
//---------------------------------------------------------------------------------------------------
EntityClassExist::EntityClassExist(PlayerType p_player) : ConditionEx(p_player, CONDEX_EntityClassExist)
{
    _conditionParameters[PARAM_EntityClassId] = DONT_CARE;
    _conditionParameters[PARAM_Amount] = DONT_CARE;

    _oneUse = false;
    _used = false;
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
    m_dest->_oneUse = _oneUse;
    m_dest->_used = _used;
}
//---------------------------------------------------------------------------------------------------
bool EntityClassExist::Consume(int p_amount)
{
    if (_conditionParameters[PARAM_Amount] == DONT_CARE || p_amount == DONT_CARE)
    {
        return true;
    }
    else
    {
        if (_oneUse)
        {
            if (!_used)
            {
                _used = true;
                return true;
            }
            else
            {
                return false;
            }
        }

        return true;
    }
}
//---------------------------------------------------------------------------------------------------
