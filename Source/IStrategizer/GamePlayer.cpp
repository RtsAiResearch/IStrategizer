#include <cassert>
#include "GamePlayer.h"
#include "GameEntity.h"
#include "PlayerResources.h"
#include "EngineAssist.h"
#include "GameStateEx.h"
#include "Toolbox.h"
#include "GameTechTree.h"
#include "IMSystemManager.h"
#include "DataMessage.h"
#include "GameStateEx.h"
#include "MessagePump.h"
#include "Logger.h"

using namespace IStrategizer;

GamePlayer::GamePlayer(RtsGame& p_RtsGame) : m_pState(new GameStateEx()), m_pResources(nullptr), m_pTechTree(nullptr)
{
    MessagePump::Instance(p_RtsGame).RegisterForMessage(p_RtsGame, MSG_EntityCreate, this);
    MessagePump::Instance(p_RtsGame).RegisterForMessage(p_RtsGame, MSG_EntityDestroy, this);
    MessagePump::Instance(p_RtsGame).RegisterForMessage(p_RtsGame, MSG_EntityRenegade, this);
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
    assert(m_pResources != nullptr);
    return m_pResources;
}
//////////////////////////////////////////////////////////////////////////
GameTechTree* GamePlayer::TechTree() const
{
    assert(m_pTechTree != nullptr);
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
        assert(pEntity);
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
void GamePlayer::NotifyMessegeSent(RtsGame& p_RtsGame, Message* p_Message)
{
    switch (p_Message->MessageTypeID())
    {
    case MSG_EntityRenegade:
        OnEntityRenegade(p_RtsGame, p_Message);
        break;

    case MSG_EntityCreate:
        OnEntityCreate(p_RtsGame, p_Message);
        break;

    case MSG_EntityDestroy:
        OnEntityDestroy(p_RtsGame, p_Message);
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::OnEntityCreate(RtsGame& p_RtsGame, Message* p_Message)
{
    GameEntity *pEntity = nullptr;
    TID entityId;
    EntityCreateMessage *pCreateMsg = nullptr;

    pCreateMsg = (EntityCreateMessage*)p_Message;

    if (pCreateMsg->Data()->OwnerId == m_id)
    {
        entityId = pCreateMsg->Data()->EntityId;

        if (m_entities.Contains(entityId))
        {
            LogError("Entity %d already exist in Player %s units", entityId, Enums[m_id]);
            return;
        }

        pEntity = FetchEntity(entityId);
        assert(pEntity);
        
        m_entities[entityId] = pEntity;

        LogInfo("[%s] Unit '%s':%d created at <%d, %d>",
            Enums[m_id], Enums[pEntity->Type()], pEntity->Id(), pEntity->Attr(EOATTR_PosX), pEntity->Attr(EOATTR_PosY));

        g_IMSysMgr.RegisterGameObj(p_RtsGame, entityId, pCreateMsg->Data()->OwnerId);
    }

}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::OnEntityDestroy(RtsGame& p_RtsGame, Message* p_Message)
{
    EntityDestroyMessage *pDestroyMsg = nullptr;
    GameEntity *pEntity = nullptr;
    TID entityId;

    pDestroyMsg = (EntityDestroyMessage*)p_Message;

    if (pDestroyMsg->Data()->OwnerId == m_id)
    {
        entityId = pDestroyMsg->Data()->EntityId;
        assert(m_entities.Contains(entityId));
        pEntity = GetEntity(entityId);
        assert(pEntity);
        m_entities.erase(entityId);

        g_IMSysMgr.UnregisterGameObj(entityId);

        LogInfo("[%s] Unit '%s':%d destroyed",
            Enums[m_id], Enums[pEntity->Type()], pEntity->Id());

        Toolbox::MemoryClean(pEntity);
    }
}
//////////////////////////////////////////////////////////////////////////
void GamePlayer::OnEntityRenegade(RtsGame& p_RtsGame, Message* p_Message)
{
    EntityRenegadeMessage *pRenMsg = nullptr;
    GameEntity *pEntity = nullptr;
    TID entityId;

    pRenMsg = (EntityRenegadeMessage*)p_Message;

    entityId = pRenMsg->Data()->EntityId;

    // I am the unit new owner
    if (pRenMsg->Data()->OwnerId == m_id)
    {
        assert(!m_entities.Contains(entityId));

        pEntity = FetchEntity(entityId);
        assert(pEntity);

        m_entities[entityId] = pEntity;

        LogInfo("[%s] Unit '%s':%d renegaded TO me",
            Enums[m_id], Enums[pEntity->Type()], pEntity->Id());

        g_IMSysMgr.RegisterGameObj(p_RtsGame, entityId, pRenMsg->Data()->OwnerId);
    }
    // Used to be my unit, but it is not anymore
    else if (pRenMsg->Data()->OwnerId != m_id && m_entities.Contains(entityId))
    {
        pEntity = GetEntity(entityId);
        assert(pEntity);

        m_entities.erase(entityId);

        g_IMSysMgr.UnregisterGameObj(entityId);

        LogInfo("[%s] Unit '%s':%d renegaded from me",
            Enums[m_id], Enums[pEntity->Type()], pEntity->Id());

        Toolbox::MemoryClean(pEntity);
    }
}
//////////////////////////////////////////////////////////////////////////
