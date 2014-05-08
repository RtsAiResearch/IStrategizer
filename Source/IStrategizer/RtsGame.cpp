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
const float RtsGame::MineralsPerWorkerPerFrame = 0.045f;
const float RtsGame::GasPerWorkerPerFrame = 0.07f;

MapEx<EntityClassType, GameType*> RtsGame::sm_entityTypes;
MapEx<ResearchType, GameResearch*> RtsGame::sm_researches;
bool RtsGame::sm_gameTypesInitialized = false;

RtsGame::~RtsGame()
{
    Finalize();
}
//----------------------------------------------------------------------------------------------
void RtsGame::Init()
{
    if (!sm_gameTypesInitialized)
    {
        InitializeTypes();
        InitializeResearches();
        sm_gameTypesInitialized = true;
    }

    _ASSERTE(!m_initialized);
    if (!m_initialized)
    {
        InitializePlayers();
        InitializeMap();
        m_initialized = true;
    }
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
    for(MapEx<EntityClassType, GameType*>::iterator itr = sm_entityTypes.begin();
        itr != sm_entityTypes.end();
        ++itr)
    {
        itr->second = FetchEntityType(itr->first);
    }
}
//----------------------------------------------------------------------------------------------
void RtsGame::InitializeResearches() 
{
    EnumerateResearches();
    for(MapEx<ResearchType, GameResearch*>::iterator itr = sm_researches.begin();
        itr != sm_researches.end();
        ++itr)
    {
        itr->second = FetchResearch(itr->first);
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
    sm_entityTypes.Keys(p_entityTypeIds);
}
//----------------------------------------------------------------------------------------------
void RtsGame::Researches(vector<ResearchType>& p_researchTypeIds)
{
    sm_researches.Keys(p_researchTypeIds);
}
//----------------------------------------------------------------------------------------------
GamePlayer* RtsGame::GetPlayer(PlayerType p_id)
{
    _ASSERTE(m_initialized);

    if (m_players.Contains(p_id))
        return m_players[p_id];
    else
        DEBUG_THROW(ItemNotFoundException(XcptHere));
}
//----------------------------------------------------------------------------------------------
GameType* RtsGame::GetEntityType(EntityClassType p_id)
{
    _ASSERTE(m_initialized);
    
    if (sm_entityTypes.Contains(p_id))
        return sm_entityTypes[p_id];

    return nullptr;
}
//----------------------------------------------------------------------------------------------
GameResearch* RtsGame::GetResearch(ResearchType p_id)
{
    _ASSERTE(m_initialized);

    if (sm_researches.Contains(p_id))
        return sm_researches[p_id];

    return nullptr;
}
//----------------------------------------------------------------------------------------------
WorldMap* RtsGame::Map()
{
    _ASSERTE(m_initialized);
    
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
}//----------------------------------------------------------------------------------------------
float IStrategizer::RtsGame::GetResourceConsumbtionRatePerWorker(ResourceType p_id)
{
	switch(p_id)
	{
	case RESOURCE_Primary:
		return MineralsPerWorkerPerFrame;
	case RESOURCE_Secondary:
		return GasPerWorkerPerFrame;
	default:
		throw InvalidParameterException(XcptHere);
	}
}
//----------------------------------------------------------------------------------------------
ForceSizeType RtsGame::GetForceSizeType(int forceCount)
{
    int period = GetMaxForceSize() / 3;

    if (forceCount == 1)
        return FORCESIZE_SmallForce;
    else if (forceCount <= period)
        return FORCESIZE_MediumForce;
    else
    {
        return FORCESIZE_LargeForce;
    }
}