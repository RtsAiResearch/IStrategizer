#ifndef RTSGAME_H
#include "RtsGame.h"
#endif
#ifndef METADATA_H
#include "MetaData.h"
#endif
#ifndef GAMEPLAYER_H
#include "GamePlayer.h"
#endif
#ifndef GAMEENTITY_H
#include "GameEntity.h"
#endif
#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif
#ifndef WORLDMAP_H
#include "WorldMap.h"
#endif
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif
#ifndef ENGINEASSIST_H
#include "EngineAssist.h"
#endif
#ifndef GAMETYPE_H
#include "GameType.h"
#endif
#include "GameResearch.h"

using namespace IStrategizer;

#include <vector>
#include <cassert>
using namespace std;

IStrategizer::RtsGame* g_Game = nullptr;


RtsGame::~RtsGame()
{
    Finalize();
}
//----------------------------------------------------------------------------------------------
void RtsGame::Init()
{
    if (m_initialized)
        return;

    InitializeTypes();
    InitializeResearches();
    InitializePlayers();
    InitializeMap();

    m_initialized = true;
}
//----------------------------------------------------------------------------------------------
void RtsGame::Finalize()
{
    for (MapEx<PlayerType, GamePlayer*>::iterator itr = m_players.begin();
        itr != m_players.end(); ++itr)
        Toolbox::MemoryClean(itr->second);
    m_players.clear();

    Toolbox::MemoryClean(m_pMap);
}
//----------------------------------------------------------------------------------------------
void RtsGame::InitializeTypes()
{
    EnumerateEntityTypes();
    for(TypesMap::iterator itr = m_entityTypes.begin();
        itr != m_entityTypes.end();
        ++itr)
    {
        itr->second = GameTypeStrongPtr(FetchEntityType(itr->first));
    }
}
//----------------------------------------------------------------------------------------------
void RtsGame::InitializeResearches() 
{
    EnumerateResearches();
    for(ResearchesMap::iterator itr = m_researches.begin();
        itr != m_researches.end();
        ++itr)
    {
        itr->second = GameResearchStrongPtr(FetchResearch(itr->first));
    }
}
//----------------------------------------------------------------------------------------------
void RtsGame::InitializePlayers() 
{
    EnumeratePlayers();
    for(MapEx<PlayerType, GamePlayer*>::iterator itr = m_players.begin();
        itr != m_players.end();
        ++itr)
    {
        itr->second = FetchPlayer(itr->first);
    }
}
//----------------------------------------------------------------------------------------------
void RtsGame::Players(vector<PlayerType>& p_playerIds)
{
    m_players.Keys(p_playerIds);
}
//----------------------------------------------------------------------------------------------
void RtsGame::EntityTypes(vector<EntityClassType>& p_entityTypeIds)
{
    m_entityTypes.Keys(p_entityTypeIds);
}
//----------------------------------------------------------------------------------------------
void RtsGame::Researches(vector<ResearchType>& p_researchTypeIds)
{
    m_researches.Keys(p_researchTypeIds);
}
//----------------------------------------------------------------------------------------------
GamePlayer* RtsGame::GetPlayer(PlayerType p_id)
{
    assert(m_initialized);

    if (m_players.Contains(p_id))
        return m_players[p_id];
    else
        throw ItemNotFoundException(XcptHere);
}
//----------------------------------------------------------------------------------------------
GameTypeStrongPtr RtsGame::GetEntityType(EntityClassType p_id)
{
    assert(m_initialized);
    
    if (m_entityTypes.Contains(p_id))
        return m_entityTypes[p_id];

    return nullptr;
}
//----------------------------------------------------------------------------------------------
GameResearchStrongPtr RtsGame::GetResearch(ResearchType p_id)
{
    assert(m_initialized);

    if (m_researches.Contains(p_id))
        return m_researches[p_id];

    return nullptr;
}
//----------------------------------------------------------------------------------------------
WorldMap* RtsGame::Map()
{
    assert(m_initialized);

    return m_pMap;
}
//----------------------------------------------------------------------------------------------
GamePlayer* RtsGame::Self()
{
    return GetPlayer(PLAYER_Self);
}
//----------------------------------------------------------------------------------------------
GamePlayer* RtsGame::Enemy()
{
    return GetPlayer(PLAYER_Enemy);
}
//----------------------------------------------------------------------------------------------
int RtsGame::GetForceSizeCount(ForceSizeType p_forceSizeType)
{
    int period = GetMaxForceSize() / 3;

    switch (p_forceSizeType)
    {
    case FORCESIZE_SmallForce:
        return 1;
    case FORCESIZE_MediumForce:
        return period;
    case FORCESIZE_LargeForce: 
        return period * 2;
    }

    _ASSERTE(!"Not Supported Force Size");
    return 0;
}

void RtsGame::Copy(IClonable* pDest)
{
    RtsGame* pConDest = dynamic_cast<RtsGame*>(pDest);
    _ASSERT(pConDest);

    // Note that types are shared across RtsGame instances
    // Shallow copy is sufficient, and since types instances are
    // referenced by smart pointers, them memory management is OK
    pConDest->m_entityTypes = m_entityTypes;
    pConDest->m_researches = m_researches;
    pConDest->m_initialized = m_initialized;

    for (PlayersMap::iterator itr = m_players.begin(); itr != m_players.end(); ++itr)
    {
        GamePlayer* pPlayer = dynamic_cast<GamePlayer*>(itr->second->Clone());
        _ASSERT(pPlayer);

        pConDest->m_players.insert(make_pair(itr->first, pPlayer));
    }

    pConDest->m_pMap = dynamic_cast<WorldMap*>(m_pMap->Clone());
    _ASSERT(pConDest->m_pMap);
}
