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
#include "GameTechTree.h"
#include "IMSystemManager.h"
#include "DataMessage.h"
#include "GameStateEx.h"
#include "MessagePump.h"
#include "Logger.h"
#include "AttributesMetaData.h"
#include "GameType.h"

using namespace IStrategizer;
using namespace std;

GamePlayer::GamePlayer() : m_pState(new GameStateEx()), m_pResources(nullptr), m_pTechTree(nullptr)
{
    g_MessagePump.RegisterForMessage(MSG_EntityCreate, this);
    g_MessagePump.RegisterForMessage(MSG_EntityDestroy, this);
    g_MessagePump.RegisterForMessage(MSG_EntityRenegade, this);
    m_colonyCenter = MapArea::Null();
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
    _ASSERTE(m_pResources != nullptr);
    return m_pResources;
}
//////////////////////////////////////////////////////////////////////////
GameTechTree* GamePlayer::TechTree() const
{
    _ASSERTE(m_pTechTree != nullptr);
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
    GameEntity* pEntity = nullptr;

    if(m_entities.Contains(p_id))
    {
        pEntity = m_entities[p_id];
        _ASSERTE(pEntity);
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
void GamePlayer::Entities(EntityClassType p_typeId, vector<TID> &p_entityIds)
{
    p_entityIds.clear();
    for(EntitiesMap::iterator itr = m_entities.begin(); itr != m_entities.end(); ++itr)
    {
        if (itr->second->Type() == p_typeId)
            p_entityIds.push_back(itr->first);
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
    GameEntity *pEntity = nullptr;
    TID entityId;
    EntityCreateMessage *pCreateMsg = nullptr;

    pCreateMsg = (EntityCreateMessage*)p_pMessage;

    if (pCreateMsg->Data()->OwnerId == m_id)
    {
        entityId = pCreateMsg->Data()->EntityId;

        if (m_entities.Contains(entityId))
        {
            LogError("Entity %d already exist in Player %s units", entityId, Enums[m_id]);
            return;
        }

        pEntity = FetchEntity(entityId);
        _ASSERTE(pEntity);
        
        m_entities[entityId] = pEntity;

        LogInfo("[%s] Unit '%s':%d created at <%d, %d>",
            Enums[m_id], Enums[pEntity->Type()], pEntity->Id(), pEntity->Attr(EOATTR_PosX), pEntity->Attr(EOATTR_PosY));

        g_IMSysMgr.RegisterGameObj(entityId, pCreateMsg->Data()->OwnerId);
    }

}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::OnEntityDestroy(Message* p_pMessage)
{
    EntityDestroyMessage *pDestroyMsg = nullptr;
    GameEntity *pEntity = nullptr;
    TID entityId;

    pDestroyMsg = (EntityDestroyMessage*)p_pMessage;

    if (pDestroyMsg->Data()->OwnerId == m_id)
    {
        entityId = pDestroyMsg->Data()->EntityId;
        _ASSERTE(m_entities.Contains(entityId));
        pEntity = GetEntity(entityId);
        pDestroyMsg->Data()->EntityType = pEntity->Type();
        _ASSERTE(pEntity);
        m_entities.erase(entityId);

        g_IMSysMgr.UnregisterGameObj(entityId);

        LogInfo("[%s] Unit '%s':%d destroyed",
            Enums[m_id], Enums[pEntity->Type()], pEntity->Id());

        Toolbox::MemoryClean(pEntity);
    }
}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::OnEntityRenegade(Message* p_pMessage)
{
    EntityRenegadeMessage *pRenMsg = nullptr;
    GameEntity *pEntity = nullptr;
    TID entityId;

    pRenMsg = (EntityRenegadeMessage*)p_pMessage;

    entityId = pRenMsg->Data()->EntityId;

    // I am the unit new owner
    if (pRenMsg->Data()->OwnerId == m_id)
    {
        _ASSERTE(!m_entities.Contains(entityId));

        pEntity = FetchEntity(entityId);
        _ASSERTE(pEntity);

        m_entities[entityId] = pEntity;

        LogInfo("[%s] Unit '%s':%d renegaded TO me",
            Enums[m_id], Enums[pEntity->Type()], pEntity->Id());

        g_IMSysMgr.RegisterGameObj(entityId, pRenMsg->Data()->OwnerId);
    }
    // Used to be my unit, but it is not anymore
    else if (pRenMsg->Data()->OwnerId != m_id && m_entities.Contains(entityId))
    {
        pEntity = GetEntity(entityId);
        _ASSERTE(pEntity);

        m_entities.erase(entityId);

        g_IMSysMgr.UnregisterGameObj(entityId);

        LogInfo("[%s] Unit '%s':%d renegaded from me",
            Enums[m_id], Enums[pEntity->Type()], pEntity->Id());

        Toolbox::MemoryClean(pEntity);
    }
}
//////////////////////////////////////////////////////////////////////////
MapArea GamePlayer::GetColonyMapArea()
{
    // We didn't calculate player colony center yet ?
    if (m_colonyCenter.IsNull())
    {
        GameEntity *pPlayerBase = nullptr;
        vector<TID> playerBases;

        GetBases(playerBases);

        // Player has at least one base, then we use the first one
        // Note that player having many bases it not supported by the engine
        if (!playerBases.empty())
            pPlayerBase = GetEntity(playerBases[0]);
        // No base! This is weird but for the case, we will select the first unit position as the player coloney center
        else
        {
            vector<TID>    playerEntities;

            Entities(playerEntities);
            // This can't happen, If the player has no entities, then he must be losing
            _ASSERTE(!playerEntities.empty());

            pPlayerBase = GetEntity(playerEntities[0]);
        }

        GameType *pGameType = g_Game->GetEntityType(GetBaseType());
        _ASSERTE(pGameType);

        m_colonyCenter = MapArea(
            Vector2(pPlayerBase->Attr(EOATTR_PosX), pPlayerBase->Attr(EOATTR_PosY)),
            pGameType->Attr(ECATTR_Width),
            pGameType->Attr(ECATTR_Height));
    }

    return m_colonyCenter;
}