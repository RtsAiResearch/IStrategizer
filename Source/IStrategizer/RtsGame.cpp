#include "RtsGame.h"

#include <vector>
#include <cassert>

#include "GamePlayer.h"
#include "GameEntity.h"
#include "WorldMap.h"
#include "Toolbox.h"
#include "EngineAssist.h"
#include "GameType.h"
#include "GameResearch.h"
#include "ObjectSerializer.h"
#include "Logger.h"

using namespace IStrategizer;
using namespace Serialization;
using namespace std;

IStrategizer::RtsGame* g_Game = nullptr;

SMap<EntityClassType, GameType*> RtsGame::sm_entityTypes;
SMap<ResearchType, GameResearch*> RtsGame::sm_researches;
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
        LogInfo("Initializing RTS Game model types");
        InitTypes();
        InitResearches();
        sm_gameTypesInitialized = true;
    }

    // We initialize player and map IFF we are in the online mode
    // If we are offline, then the player and map data was already
    // deserialized and no need to fetch it from somewhere
    if (m_isOnline)
    {
        LogInfo("Initializing Online RTS Game model");

        _ASSERTE(!m_initialized);
        InitPlayers();
        InitMap();
        m_initialized = true;
    }
    else
    {
        _ASSERTE(!m_initialized);
        LogInfo("RTS Game model is in Offline mode, nothing to initialize");
    }
}
//----------------------------------------------------------------------------------------------
void RtsGame::Finalize()
{
    for (auto& playerEntry : m_players)
        Toolbox::MemoryClean(playerEntry.second);
    m_players.clear();

    Toolbox::MemoryClean(m_pMap);
}
//----------------------------------------------------------------------------------------------
void RtsGame::InitTypes()
{
    EnumerateEntityTypes();
    for(auto& typeEntry : sm_entityTypes)
    {
        typeEntry.second = FetchEntityType(typeEntry.first);
    }
}
//----------------------------------------------------------------------------------------------
void RtsGame::InitResearches() 
{
    EnumerateResearches();
    for(auto& researchEntry : sm_researches)
    {
        researchEntry.second = FetchResearch(researchEntry.first);
    }
}
//----------------------------------------------------------------------------------------------
void RtsGame::InitPlayers() 
{
    EnumeratePlayers();
    for(auto& playerEntry : m_players)
    {
        playerEntry.second = FetchPlayer(playerEntry.first);
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

    if (m_players.count(p_id) > 0)
        return m_players[p_id];
    else
        DEBUG_THROW(ItemNotFoundException(XcptHere));
}
//----------------------------------------------------------------------------------------------
GameType* RtsGame::GetEntityType(EntityClassType p_id)
{
    _ASSERTE(m_initialized);

    if (sm_entityTypes.count(p_id) > 0)
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
    switch (p_forceSizeType)
    {
    case FORCESIZE_SmallForce:
        return 1;

    case FORCESIZE_MediumForce:
        return GetMaxForceSize() / 2;

    case FORCESIZE_LargeForce: 
        return GetMaxForceSize();
    }

    _ASSERTE(!"Not Supported Force Size");
    return 0;
}
//----------------------------------------------------------------------------------------------
ForceSizeType RtsGame::GetForceSizeType(int forceCount)
{
    if (forceCount == 1)
        return FORCESIZE_SmallForce;

    else if (forceCount <= GetMaxForceSize() / 2)
        return FORCESIZE_MediumForce;

    else
        return FORCESIZE_LargeForce;
}
//----------------------------------------------------------------------------------------------
RtsGame* RtsGame::Clone() const
{
    RtsGame* pClone = static_cast<RtsGame*>(Prototype());

    string tempFilename = "temp";
    tempFilename += to_string((int)time(nullptr));

    g_ObjectSerializer.Serialize(this, tempFilename);

    g_ObjectSerializer.Deserialize(pClone, tempFilename);
    pClone->m_isOnline = false;
    pClone->m_initialized = false;
    
    ifstream file(tempFilename.c_str());
    if (file.is_open())
    {
        file.close();
    }

    return pClone;
}
