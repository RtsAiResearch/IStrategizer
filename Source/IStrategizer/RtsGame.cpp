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

EntiyTypesMap RtsGame::sm_entityTypes;
ResearchTypesMap RtsGame::sm_researchTypes;
RaceTypesMap RtsGame::sm_raceTypes;
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
        InitEntityTypes();
        InitResearchTypes();
        InitRaceTypes();
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
GamePlayer* RtsGame::GetPlayer(PlayerType p_id)
{
    _ASSERTE(m_initialized);

    if (!m_players.Contains(p_id))
        DEBUG_THROW(ItemNotFoundException(XcptHere));

    return m_players[p_id];
}
//----------------------------------------------------------------------------------------------
GameType* RtsGame::GetEntityType(EntityClassType p_id)
{
    _ASSERTE(m_initialized);

    if (!sm_entityTypes.Contains(p_id))
        DEBUG_THROW(ItemNotFoundException(XcptHere));

    return sm_entityTypes[p_id];
}
//----------------------------------------------------------------------------------------------
GameResearch* RtsGame::GetResearch(ResearchType p_id)
{
    _ASSERTE(m_initialized);
    if (!sm_researchTypes.Contains(p_id))
        DEBUG_THROW(ItemNotFoundException(XcptHere));

    return sm_researchTypes[p_id];
}
//----------------------------------------------------------------------------------------------
GameRace* RtsGame::GetRace(TID id)
{
    _ASSERTE(m_initialized);
    if (!sm_raceTypes.Contains(id))
        DEBUG_THROW(ItemNotFoundException(XcptHere));

    return sm_raceTypes[id];
}
//----------------------------------------------------------------------------------------------
void RtsGame::SetOffline()
{
    // Currently, once an RTS game instance is brought offline, it can't come online again

    for (auto pPlayer : m_players)
        pPlayer.second->SetOffline(this);

    m_pMap->SetOffline(this);
}
//----------------------------------------------------------------------------------------------
RtsGame* RtsGame::Snapshot() const
{
    RtsGame* pSnapshot = dynamic_cast<RtsGame*>(Prototype());

    pSnapshot->Init();
    pSnapshot->SetOffline();

    return pSnapshot;
}
