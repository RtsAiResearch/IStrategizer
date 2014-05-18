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
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif
#include "GameTechTree.h"
#include "IMSystemManager.h"
#include "DataMessage.h"
#include "MessagePump.h"
#include "Logger.h"
#include "AttributesMetaData.h"
#include "GameType.h"
#include "IStrategizerException.h"

using namespace IStrategizer;
using namespace std;

GamePlayer::GamePlayer(TID raceId) :
    m_pResources(nullptr),
    m_pTechTree(nullptr),
    m_raceId(raceId)
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
    for (auto entityEntry : m_entities)
        Toolbox::MemoryClean(entityEntry.second);
    m_entities.clear();

    Toolbox::MemoryClean(m_pResources);
    Toolbox::MemoryClean(m_pTechTree);
}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::Entities(vector<TID>& entityIds)
{
    m_entities.Keys(entityIds);
}
//////////////////////////////////////////////////////////////////////////
GameEntity* GamePlayer::GetEntity(TID id)
{
    if(m_entities.count(id) == 0)
    {
        return nullptr;
    }

    _ASSERTE(m_entities[id] != nullptr);
    return m_entities[id];
}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::GetBases(vector<TID> &basesIds)
{
    EntityClassType typeId;

    typeId = Race()->GetBaseType();

    basesIds.clear();

    for(EntitiesMap::iterator itr = m_entities.begin();
        itr != m_entities.end(); ++itr)
    {
        if (itr->second->Type() == typeId)
            basesIds.push_back(itr->first);
    }
}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::Entities(EntityClassType typeId, vector<TID> &entityIds)
{
    entityIds.clear();
    for(EntitiesMap::iterator itr = m_entities.begin(); itr != m_entities.end(); ++itr)
    {
        if (itr->second->Type() == typeId)
            entityIds.push_back(itr->first);
    }
}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::NotifyMessegeSent(Message* pMsg)
{
    switch (pMsg->MessageTypeID())
    {
    case MSG_EntityRenegade:
        OnEntityRenegade(pMsg);
        break;

    case MSG_EntityCreate:
        OnEntityCreate(pMsg);
        break;

    case MSG_EntityDestroy:
        OnEntityDestroy(pMsg);
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::OnEntityCreate(Message* pMsg)
{
    GameEntity *pEntity = nullptr;
    TID entityId;
    EntityCreateMessage *pCreateMsg = nullptr;

    pCreateMsg = (EntityCreateMessage*)pMsg;

    if (pCreateMsg->Data()->OwnerId == m_type)
    {
        entityId = pCreateMsg->Data()->EntityId;

        if (m_entities.Contains(entityId))
        {
            LogError("Entity %d already exist in Player %s units", entityId, Enums[m_type]);
            return;
        }

        pEntity = FetchEntity(entityId);
        _ASSERTE(pEntity);

        m_entities[entityId] = pEntity;

        LogInfo("[%s] Unit '%s':%d created at <%d, %d>",
            Enums[m_type], Enums[pEntity->Type()], pEntity->Id(), pEntity->Attr(EOATTR_Left), pEntity->Attr(EOATTR_Top));

        g_IMSysMgr.RegisterGameObj(entityId, pCreateMsg->Data()->OwnerId);
    }

}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::OnEntityDestroy(Message* pMsg)
{
    EntityDestroyMessage *pDestroyMsg = nullptr;
    GameEntity *pEntity = nullptr;
    TID entityId;

    pDestroyMsg = (EntityDestroyMessage*)pMsg;

    if (pDestroyMsg->Data()->OwnerId == m_type)
    {
        entityId = pDestroyMsg->Data()->EntityId;
        _ASSERTE(m_entities.Contains(entityId));
        pEntity = GetEntity(entityId);
        pDestroyMsg->Data()->EntityType = pEntity->Type();
        _ASSERTE(pEntity);
        m_entities.erase(entityId);

        g_IMSysMgr.UnregisterGameObj(entityId);

        LogInfo("[%s] Unit '%s':%d destroyed",
            Enums[m_type], Enums[pEntity->Type()], pEntity->Id());

        Toolbox::MemoryClean(pEntity);
    }
}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::OnEntityRenegade(Message* pMsg)
{
    EntityRenegadeMessage *pRenMsg = nullptr;
    GameEntity *pEntity = nullptr;
    TID entityId;

    pRenMsg = (EntityRenegadeMessage*)pMsg;

    entityId = pRenMsg->Data()->EntityId;

    // I am the unit new owner
    if (pRenMsg->Data()->OwnerId == m_type)
    {
        _ASSERTE(!m_entities.Contains(entityId));

        pEntity = FetchEntity(entityId);
        _ASSERTE(pEntity);

        m_entities[entityId] = pEntity;

        LogInfo("[%s] Unit '%s':%d renegaded TO me",
            Enums[m_type], Enums[pEntity->Type()], pEntity->Id());

        g_IMSysMgr.RegisterGameObj(entityId, pRenMsg->Data()->OwnerId);
    }
    // Used to be my unit, but it is not anymore
    else if (pRenMsg->Data()->OwnerId != m_type && m_entities.Contains(entityId))
    {
        pEntity = GetEntity(entityId);
        _ASSERTE(pEntity);

        m_entities.erase(entityId);

        g_IMSysMgr.UnregisterGameObj(entityId);

        LogInfo("[%s] Unit '%s':%d renegaded from me",
            Enums[m_type], Enums[pEntity->Type()], pEntity->Id());

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

        GameType *pGameType = g_Game->GetEntityType(Race()->GetBaseType());
        _ASSERTE(pGameType);

        m_colonyCenter = MapArea(
            Vector2(pPlayerBase->Attr(EOATTR_Left), pPlayerBase->Attr(EOATTR_Top)),
            pGameType->Attr(ECATTR_Width),
            pGameType->Attr(ECATTR_Height));
    }

    return m_colonyCenter;
}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::SetOffline(RtsGame* pBelongingGame)
{
    g_MessagePump.UnregisterForAllMessages(this);

    m_pResources->SetOffline(pBelongingGame);
    m_pTechTree->SetOffline(pBelongingGame);
}
