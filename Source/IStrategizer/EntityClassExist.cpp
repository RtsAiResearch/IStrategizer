#include "EntityClassExist.h"
#include "CheckEntityObjectSimilarity.h"
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

EntityClassExist::EntityClassExist(PlayerType p_player, EntityClassType p_unitClassId, int p_amount, map< EntityObjectAttribute,int > p_entityObjectAttributes, int p_similarityThreshold, bool p_blocking) 
: ConditionEx(p_player, CONDEX_EntityClassExist)
{
	_conditionParameters[PARAM_EntityClassId] = p_unitClassId;
	_conditionParameters[PARAM_Amount] = p_amount;
	_similarityThreshold = p_similarityThreshold;
	_entityObjectAttributes.insert(p_entityObjectAttributes.begin(), p_entityObjectAttributes.end());

	_oneUse		= false;
	_used		= false;
	_blocking	= p_blocking;

}
//---------------------------------------------------------------------------------------------------
EntityClassExist::EntityClassExist(PlayerType p_player, EntityClassType p_unitClassId, int p_amount, bool p_oneUse, bool p_blocking) : ConditionEx(p_player, CONDEX_EntityClassExist)
{
	_conditionParameters[PARAM_EntityClassId] = p_unitClassId;
	_conditionParameters[PARAM_Amount] = p_amount;

	_oneUse		= p_oneUse;
	_used		= false;
	_blocking	= p_blocking;
}
//----------------------------------------------------------------------------------------------
void EntityClassExist::InitializeAddressesAux()
{
	ConditionEx::InitializeAddressesAux();
	AddMemberAddress(4,
		&_entityObjectAttributes,
		&_similarityThreshold,
		&_oneUse,
		&_used);
}
//---------------------------------------------------------------------------------------------------
bool EntityClassExist::Evaluate()
{
	int					m_entityObjectId;
	vector<TID>	m_execluded;
	GameEntity*			m_entity;
	GamePlayer*			m_player;

	ConditionEx::Evaluate();
	vector<int> m_entityIds;
	m_player = g_Game->GetPlayer((PlayerType)_conditionParameters[PARAM_PlayerId]);
	m_player->Entities(m_entityIds);

	if(!m_entityIds.empty())
	{
		_entityObjectIds.clear();
		_isSatisfied = false;

		int m_amount = _conditionParameters[PARAM_Amount];
		for (unsigned m_index = 0; m_index < m_entityIds.size(); m_index++)
		{
			m_entityObjectId = m_entityIds[m_index];

			m_entity = m_player->GetEntity(m_entityObjectId);

			if(_blocking && m_entity->Locked())
				continue;

			if (m_entity->Type() == _conditionParameters[PARAM_EntityClassId] && MatchesAttributes(m_entityObjectId))
			{
				if(_blocking)
					m_entity->Lock();

				_entityObjectIds.push_back(m_entityObjectId);

				if(--m_amount == 0)
				{
					_isSatisfied = true;
					break;
				}
			}
		}
	}
	/*for (int m_index = 0; _conditionParameters[PARAM_Amount] > _entityObjectIds.size() && _isSatisfied; m_index++)
	{
	m_returnValue = g_EngineAPI.GetEntityObjectFromEntityClass(_playerType, (EntityClassType)_conditionParameters[PARAM_EntityClassId], m_execluded, m_entityObjectId);

	m_entity = m_player->GetEntity(m_entityObjectId);

	if(_blocking && m_entity->Locked())
	continue;

	if (m_returnValue == ERR_Success)
	{
	if (MatchesAttributes(m_entityObjectId))
	{
	if(_blocking)
	m_entity->Lock();

	_entityObjectIds.push_back(m_entityObjectId);
	m_execluded.push_back(m_entityObjectId);
	}
	else
	{
	m_execluded.push_back(m_entityObjectId);
	}
	}
	else if(m_returnValue == ERR_EntityDoesNotExist)
	{
	_isSatisfied = false;
	break;
	}
	}*/

	return _isEvaluated && _isSatisfied;
}
//---------------------------------------------------------------------------------------------------
bool EntityClassExist::MatchesAttributes(int p_entityObjectId)
{   
	return true;

	if(_entityObjectAttributes.size() == 0)
		return true;
	else
		return CheckEntityObjectSimilarity(_playerType, p_entityObjectId, _entityObjectAttributes, _similarityThreshold).Evaluate();
}
//---------------------------------------------------------------------------------------------------
void EntityClassExist::Copy(IClonable* p_dest)
{
	ConditionEx::Copy(p_dest);

	EntityClassExist* m_dest = static_cast<EntityClassExist*>(p_dest);

	m_dest->_entityObjectIds          = _entityObjectIds;
	m_dest->_entityObjectAttributes   = _entityObjectAttributes;
	m_dest->_similarityThreshold      = _similarityThreshold;
	m_dest->_oneUse					  = _oneUse;
	m_dest->_used					  = _used;
}
//---------------------------------------------------------------------------------------------------
bool EntityClassExist::Consume(int p_amount)
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
//---------------------------------------------------------------------------------------------------
