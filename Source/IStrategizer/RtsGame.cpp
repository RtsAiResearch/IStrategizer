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
        InitEntityTypes();
        InitResearchTypes();
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
    if (!sm_researches.Contains(p_id))
        DEBUG_THROW(ItemNotFoundException(XcptHere));

    return sm_researches[p_id];
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
RtsGame* RtsGame::Clone() const
{
    string tempFilename = "temp";
    tempFilename += to_string((int)time(nullptr));

    // The Copy/Paste trick is to serialize and deserialize the same object
    g_ObjectSerializer.Serialize(this, tempFilename);

    RtsGame* pClone = static_cast<RtsGame*>(Prototype());
    g_ObjectSerializer.Deserialize(pClone, tempFilename);

    pClone->SetOffline();

    ifstream file(tempFilename.c_str());
    if (file.is_open())
    {
        file.close();
        LogWarning("LEAK: A temp file was created and was not deleted");
    }

    return pClone;
}
