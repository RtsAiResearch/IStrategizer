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
#include "SimilarityWeightModel.h"

using namespace IStrategizer;
using namespace Serialization;
using namespace std;

IStrategizer::RtsGame* g_Game = nullptr;

EntiyTypesMap RtsGame::sm_entityTypes;
ResearchTypesMap RtsGame::sm_researchTypes;
RaceTypesMap RtsGame::sm_raceTypes;
bool RtsGame::sm_gameTypesInitialized = false;

static SimilarityWeightModel GetDefaultWeightModel()
{
    SimilarityWeightModel model;

    model.GameFrame = 0.2f;
    model.MapArea = 0.2f;
    model.Player.W = 0.6f;
        model.Player.Entities.W = 0.6f;
            model.Player.Entities.NumAttackingUnits = 0.25;
            model.Player.Entities.NumBuildings = 0.25;
            model.Player.Entities.NumWorkers = 0.25;
            model.Player.Entities.NumDoneResearches = 0.25;
        model.Player.Resources.W = 0.4f;
            model.Player.Resources.Primary = 0.3f;
            model.Player.Resources.Secondary = 0.3f;
            model.Player.Resources.Supply = 0.4f;

    return model;
}

SimilarityWeightModel RtsGame::DefaultWeightModel = GetDefaultWeightModel();

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
        _ASSERTE(!m_isInitialized);
        InitPlayers();
        InitMap();
        m_isInitialized = true;
    }
    else
    {
        _ASSERTE(!m_isInitialized);
        InitMap();
        m_isInitialized = true;
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
GamePlayer* RtsGame::GetPlayer(PlayerType p_id) const
{
    _ASSERTE(m_isInitialized);

    if (!m_players.Contains(p_id))
        DEBUG_THROW(ItemNotFoundException(XcptHere));

    return m_players.at(p_id);
}
//----------------------------------------------------------------------------------------------
GameType* RtsGame::GetEntityType(EntityClassType p_id)
{
    _ASSERTE(m_isInitialized);

    if (!sm_entityTypes.Contains(p_id))
        DEBUG_THROW(ItemNotFoundException(XcptHere));

    return sm_entityTypes[p_id];
}
//----------------------------------------------------------------------------------------------
GameResearch* RtsGame::GetResearch(ResearchType p_id)
{
    _ASSERTE(m_isInitialized);
    if (!sm_researchTypes.Contains(p_id))
        DEBUG_THROW(ItemNotFoundException(XcptHere));

    return sm_researchTypes[p_id];
}
//----------------------------------------------------------------------------------------------
GameRace* RtsGame::GetRace(TID id)
{
    _ASSERTE(m_isInitialized);
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

    m_cachedGameFrame = GameFrame();

    m_isOnline = false;
}
//----------------------------------------------------------------------------------------------
RtsGame* RtsGame::Snapshot() const
{
    RtsGame* pSnapshot = dynamic_cast<RtsGame*>(Prototype());

    pSnapshot->Init();
    pSnapshot->SetOffline();

    return pSnapshot;
}
//----------------------------------------------------------------------------------------------
float RtsGame::Distance(const RtsGame* pOther, const SimilarityWeightModel* pModel) const
{
    float distance = 0;

    distance += pModel->GameFrame * abs(
        int(GameFrame()) - int(pOther->GameFrame()));

    distance += pModel->MapArea * abs(
        int(Map()->Area()) - int(pOther->Map()->Area()));

    distance += Self()->Distance(pOther->Self(), pModel);

    return distance;
}