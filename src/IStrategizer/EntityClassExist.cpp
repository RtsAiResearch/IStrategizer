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

EntityClassExist::EntityClassExist(PlayerType p_player, EntityClassType p_unitClassId, int p_amount, ObjectStateType state) :
ConditionEx(p_player, CONDEX_EntityClassExist)
{
    _conditionParameters[PARAM_EntityClassId] = p_unitClassId;
    _conditionParameters[PARAM_Amount] = p_amount;
    _conditionParameters[PARAM_ObjectStateType] = state;
}
//---------------------------------------------------------------------------------------------------
EntityClassExist::EntityClassExist(PlayerType p_player, EntityClassType p_unitClassId, ObjectStateType state) : ConditionEx(p_player, CONDEX_EntityClassExist)
{
    _conditionParameters[PARAM_EntityClassId] = p_unitClassId;
    _conditionParameters[PARAM_Amount] = DONT_CARE;
    _conditionParameters[PARAM_ObjectStateType] = state;
}
//---------------------------------------------------------------------------------------------------
EntityClassExist::EntityClassExist(PlayerType p_player) : ConditionEx(p_player, CONDEX_EntityClassExist)
{
    _conditionParameters[PARAM_EntityClassId] = OBJSTATE_END;
    _conditionParameters[PARAM_Amount] = DONT_CARE;
}
//---------------------------------------------------------------------------------------------------
bool EntityClassExist::Evaluate(RtsGame& game)
{
    if (_conditionParameters[PARAM_EntityClassId] != OBJSTATE_END)
    {
        EntityClassType entityClassId = (EntityClassType)_conditionParameters[PARAM_EntityClassId];
        int amount = _conditionParameters[PARAM_Amount];
        PlayerType playerId = (PlayerType)_conditionParameters[PARAM_PlayerId];

        ConditionEx::Evaluate(game);
        _isSatisfied = g_Assist.DoesEntityClassExist(MakePair(entityClassId, amount), (ObjectStateType)_conditionParameters[PARAM_ObjectStateType], playerId);
    }

    return _isEvaluated && _isSatisfied;
}
//---------------------------------------------------------------------------------------------------
bool EntityClassExist::Consume(int p_amount)
{
    if (p_amount > 0 || _conditionParameters[PARAM_Amount] == DONT_CARE)
    {
        // (1) The entity is reusable so return true
        // There is no required amount to so return true.
        // (2) The source is infinite so always return true
        return true;
    }
    else if (p_amount == 0 || _conditionParameters[PARAM_Amount] == 0)
    {
        // The current available amount is 0 so can not consume anything.
        return false;
    }
    else if (p_amount == DONT_CARE && _conditionParameters[PARAM_Amount] != DONT_CARE)
    {
        // The request is infinite source, consume all available amount
        _ASSERTE(_conditionParameters[PARAM_Amount] > 0);
        _conditionParameters[PARAM_Amount] = 0;

        return true;
    }
    else
    {
        _ASSERTE(p_amount > 0 && _conditionParameters[PARAM_Amount] > 0);
        _conditionParameters[PARAM_Amount] -= p_amount;

        return true;
    }
}
