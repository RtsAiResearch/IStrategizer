#ifndef GAMEPLAYER_H
#include "GamePlayer.h"
#endif
#include <cassert>
#ifndef GAMEENTITY_H
#include "GameEntity.h"
#endif
#ifndef PLAYERRESOURCES_H
#include "PlayerResources.h"
#endif
#ifndef ENGINEASSIST_H
#include "EngineAssist.h"
#endif
#ifndef GAMESTATEEX_H
#include "GameStateEx.h"
#endif
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif
#include "IMSystemManager.h"
#include "DataMessage.h"
#include "GameStateEx.h"
#include "MessagePump.h"

using namespace IStrategizer;
using namespace DataStructure;
using namespace IStrategizer;

GamePlayer::GamePlayer() : m_pState(new GameStateEx()), m_pResources(NULL), m_pTechTree(NULL)
{
	g_MessagePump.RegisterForMessage(MSG_EntityCreate, this);
	g_MessagePump.RegisterForMessage(MSG_EntityDestroy, this);
	g_MessagePump.RegisterForMessage(MSG_EntityRenegade, this);
}
//////////////////////////////////////////////////////////////////////////
GamePlayer::~GamePlayer()
{
	Finalize();
}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::Finalize()
{
	for (MapEx<TID, GameEntity*>::iterator itr = m_entities.begin();
		itr != m_entities.end(); ++itr)
		Toolbox::MemoryClean(itr->second);
	m_entities.clear();

	Toolbox::MemoryClean(m_pResources);
	Toolbox::MemoryClean(m_pTechTree);
	Toolbox::MemoryClean(m_pState);
}
//////////////////////////////////////////////////////////////////////////
PlayerResources* GamePlayer::Resources()
{
	assert(m_pResources != NULL);
	return m_pResources;
}
//////////////////////////////////////////////////////////////////////////
GameTechTree* GamePlayer::TechTree() const
{
	assert(m_pTechTree != NULL);
	return m_pTechTree;
}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::Entities(vector<TID>& p_entityIds)
{
	m_entities.Keys(p_entityIds);
}
//////////////////////////////////////////////////////////////////////////
GameEntity* GamePlayer::GetEntity(TID p_id)
{
	GameEntity* pEntity = NULL;

	if(m_entities.Contains(p_id))
	{
		pEntity = m_entities[p_id];
		assert(pEntity);
	}
	else
	{
		pEntity = FetchEntity(p_id);
		assert(pEntity);
		m_entities[p_id] = pEntity;
	}

	return pEntity;
}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::GetBases(vector<TID> &p_basesIds)
{
	EntityClassType typeId;

	typeId = GetBaseType();

	p_basesIds.clear();

	for(EntitiesMap::iterator itr = m_entities.begin();
		itr != m_entities.end(); ++itr)
	{
		if (itr->second->Type() == typeId)
			p_basesIds.push_back(itr->first);
	}
}
//////////////////////////////////////////////////////////////////////////
const GameStateEx* GamePlayer::State()
{
	return m_pState;
}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::NotifyMessegeSent(Message* p_pMessage)
{
	switch (p_pMessage->MessageTypeID())
	{
	case MSG_EntityRenegade:
		OnEntityRenegade(p_pMessage);
		break;

	case MSG_EntityCreate:
		OnEntityCreate(p_pMessage);
		break;

	case MSG_EntityDestroy:
		OnEntityDestroy(p_pMessage);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::OnEntityCreate(Message* p_pMessage)
{
	GameEntity				*pEntity = NULL;
	TID						entityId;
	EntityCreateMessage		*pCreateMsg = NULL;

	pCreateMsg = (EntityCreateMessage*)p_pMessage;

	if (pCreateMsg->Data()->OwnerId == m_id)
	{
		entityId = pCreateMsg->Data()->EntityId;
		assert(!m_entities.Contains(entityId));

		pEntity = FetchEntity(entityId);
		assert(pEntity);

		m_entities[entityId] = pEntity;

		printf("[%s] Unit '%s':%d created at <%d, %d>\n",
			Enums[m_id], Enums[pEntity->Type()], pEntity->Id(), pEntity->Attr(EOATTR_PosX), pEntity->Attr(EOATTR_PosY));

		g_IMSysMgr.RegisterGameObj(entityId, pCreateMsg->Data()->OwnerId);
	}

}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::OnEntityDestroy(Message* p_pMessage)
{
	EntityDestroyMessage	*pDestroyMsg = NULL;
	GameEntity				*pEntity = NULL;
	TID						entityId;

	pDestroyMsg = (EntityDestroyMessage*)p_pMessage;

	if (pDestroyMsg->Data()->OwnerId == m_id)
	{
		entityId = pDestroyMsg->Data()->EntityId;
		assert(m_entities.Contains(entityId));
		pEntity = GetEntity(entityId);
		assert(pEntity);
		m_entities.erase(entityId);

		g_IMSysMgr.UnregisterGameObj(entityId);

		printf("[%s] Unit '%s':%d destroyed\n",
			Enums[m_id], Enums[pEntity->Type()], pEntity->Id());

		Toolbox::MemoryClean(pEntity);
	}
}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::OnEntityRenegade(Message* p_pMessage)
{
	EntityRenegadeMessage	*pRenMsg = NULL;
	GameEntity				*pEntity = NULL;
	TID						entityId;

	pRenMsg = (EntityRenegadeMessage*)p_pMessage;

	entityId = pRenMsg->Data()->EntityId;

	// I am the unit new owner
	if (pRenMsg->Data()->OwnerId == m_id)
	{
		assert(!m_entities.Contains(entityId));

		pEntity = FetchEntity(entityId);
		assert(pEntity);

		m_entities[entityId] = pEntity;

		printf("[%s] Unit '%s':%d renegaded TO me\n",
			Enums[m_id], Enums[pEntity->Type()], pEntity->Id());

		g_IMSysMgr.RegisterGameObj(entityId, pRenMsg->Data()->OwnerId);
	}
	// Used to be my unit, but it is not anymore
	else if (pRenMsg->Data()->OwnerId != m_id && m_entities.Contains(entityId))
	{
		pEntity = GetEntity(entityId);
		assert(pEntity);

		m_entities.erase(entityId);

		g_IMSysMgr.UnregisterGameObj(entityId);

		printf("[%s] Unit '%s':%d renegaded from me\n",
			Enums[m_id], Enums[pEntity->Type()], pEntity->Id());

		Toolbox::MemoryClean(pEntity);
	}
}
//////////////////////////////////////////////////////////////////////////
