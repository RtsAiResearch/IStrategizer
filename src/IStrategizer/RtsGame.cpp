#include "RtsGame.h"

#include <vector>
#include <cassert>

#include "GamePlayer.h"
#include "GameEntity.h"
#include "WorldMap.h"
#include "GameType.h"
#include "GameResearch.h"
#include "ObjectSerializer.h"
#include "Logger.h"
#include "SimilarityWeightModel.h"
#include "PlayerResources.h"
#include "ObjectFactory.h"
#include "EngineDefs.h"
#include "toolbox.h"
#include "RtsAiEngine.h"
#include "IStrategizerEx.h"

using namespace IStrategizer;
using namespace Serialization;
using namespace std;

RtsGame* g_Game = nullptr;
IRtsGame* g_GameImpl = nullptr;

RtsGameStaticData* RtsGame::sm_pGameStatics = nullptr;

DECL_SERIALIZABLE(RtsGameStaticData);

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

RtsGameStaticData::~RtsGameStaticData()
{
    for (auto& entityType : EntityTypes)
        SAFE_DELETE(entityType.second);
    EntityTypes.clear();

    for (auto& researchType : ResearchTypes)
        SAFE_DELETE(researchType.second);
    ResearchTypes.clear();

    for (auto& raceType : RaceTypes)
        SAFE_DELETE(raceType.second);
    RaceTypes.clear();
}
//////////////////////////////////////////////////////////////////////////
RtsGame::RtsGame() :
m_pMap(nullptr),
m_isInitialized(false),
m_isOnline(true),
m_cachedGameFrame(0),
m_firstUpdate(true)
{}
//////////////////////////////////////////////////////////////////////////
RtsGame::~RtsGame()
{
    Finalize();
}
//////////////////////////////////////////////////////////////////////////
bool RtsGame::Init()
{
    if (nullptr == sm_pGameStatics)
    {
        LogInfo("Initializing RTS game static data");

        if (InitStaticData())
        {
            _ASSERTE(sm_pGameStatics);
            LogInfo("Game static data initialized successfully");
        }
        else
        {
            LogError("Failed to initialize game static data");
            return false;
        }
    }

    if (m_isInitialized)
        return true;

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

    return true;
}
//////////////////////////////////////////////////////////////////////////
void RtsGame::Finalize()
{
    for (auto& playerEntry : m_players)
        SAFE_DELETE(playerEntry.second);
    m_players.clear();

    SAFE_DELETE(m_pMap);
}
//////////////////////////////////////////////////////////////////////////
GamePlayer* RtsGame::GetPlayer(PlayerType p_id) const
{
    _ASSERTE(m_isInitialized);

    if (!m_players.Contains(p_id))
        return nullptr;

    return m_players.at(p_id);
}
//////////////////////////////////////////////////////////////////////////
GameType* RtsGame::GetEntityType(EntityClassType p_id)
{
    _ASSERTE(sm_pGameStatics);

    if (!sm_pGameStatics->EntityTypes.Contains(p_id))
        DEBUG_THROW(ItemNotFoundException(XcptHere));

    return sm_pGameStatics->EntityTypes[p_id];
}
//////////////////////////////////////////////////////////////////////////
GameResearch* RtsGame::GetResearch(ResearchType p_id)
{
    _ASSERTE(sm_pGameStatics);
    if (!sm_pGameStatics->ResearchTypes.Contains(p_id))
        DEBUG_THROW(ItemNotFoundException(XcptHere));

    return sm_pGameStatics->ResearchTypes[p_id];
}
//////////////////////////////////////////////////////////////////////////
GameRace* RtsGame::GetRace(TID id)
{
    _ASSERTE(sm_pGameStatics);
    if (!sm_pGameStatics->RaceTypes.Contains(id))
        DEBUG_THROW(ItemNotFoundException(XcptHere));

    return sm_pGameStatics->RaceTypes[id];
}
//////////////////////////////////////////////////////////////////////////
void RtsGame::SetOffline()
{
    // Currently, once an RTS game instance is brought offline, it can't come online again

    for (auto pPlayer : m_players)
        pPlayer.second->SetOffline(this);

    m_pMap->SetOffline(this);
    m_cachedWorldWidth = Map()->Width();
    m_cachedWorldHeight = Map()->Height();
    m_cachedGameFrame = GameFrame();

    m_isOnline = false;
}
//////////////////////////////////////////////////////////////////////////
RtsGame* RtsGame::Snapshot() const
{
    RtsGame* pSnapshot = dynamic_cast<RtsGame*>(g_ObjectFactory.Create(GetObjectLayout().TypeName()));

    pSnapshot->Init();
    pSnapshot->SetOffline();

    return pSnapshot;
}
//////////////////////////////////////////////////////////////////////////
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

float RtsGameModel::Distance(_In_ const RtsGameModel& other, _In_ const RtsGameModelAttributeWeights& weights) const
{
    return 0.0f;
}

int RtsGameModel::Attr(_In_ RtsGameModelAttribute attr) const
{
    int val = 0;

    vector<ResearchType> researches;

    switch (attr)
    {
    case RTSMODATTR_GameFrame:
        val = m_game.GameFrame();
        break;
    case RTSMODATTR_MapArea:
        val = (int)m_game.Map()->Area();
        break;
    case RTSMODATTR_Player_Entities_NumBuildings:
        val = m_game.Self()->CountEntityTypes(TP_IsBuilding, 1);
        break;
    case RTSMODATTR_Player_Entities_NumWorkers:
        val = m_game.Self()->CountEntityTypes(TP_CanBuild, 1);
        break;
    case RTSMODATTR_Player_Entities_NumAttackers:
        val = m_game.Self()->CountEntityTypes(TP_IsAttacker, 1);
        break;
    case RTSMODATTR_Player_Entities_NumDoneResearches:
        for (auto& researchR : RtsGame::sm_pGameStatics->ResearchTypes)
        {
            if (m_game.Self()->TechTree()->ResearchDone(researchR.first))
                ++val;
        }
        break;
    case RTSMODATTR_Player_Resources_Primary:
        val = m_game.Self()->Resources()->Primary();
        break;
    case RTSMODATTR_Player_Resources_Secondary:
        val = m_game.Self()->Resources()->Secondary();
        break;
    case RTSMODATTR_Player_Resources_Supply:
        val = m_game.Self()->Resources()->Supply();
        break;
    default:
        DEBUG_THROW(NotImplementedException(XcptHere));
        break;
    }

    return val;
}
//////////////////////////////////////////////////////////////////////////
void RtsGame::ExportStaticData()
{
    LogInfo("Exporting game static data to disk '%s'", GAME_STATIC_DATA_FILENAME);
    g_ObjectSerializer.Serialize(sm_pGameStatics, GAME_STATIC_DATA_FILENAME);
}
//////////////////////////////////////////////////////////////////////////
void RtsGame::FinalizeStaticData()
{
    SAFE_DELETE(sm_pGameStatics);
}
//////////////////////////////////////////////////////////////////////////
GameEntity* RtsGame::GetEntity(_In_ TID entityId)
{
    for (auto& playerR : m_players)
    {
        auto pEntity = playerR.second->GetEntity(entityId);

        if (pEntity != nullptr)
            return pEntity;
    }

    return nullptr;
}
//////////////////////////////////////////////////////////////////////////
void RtsGame::DebugDraw()
{
    for (auto& playerR : m_players)
    {
        if (playerR.first == PLAYER_Neutral)
            continue;

        playerR.second->DebugDraw();
    }
}
//////////////////////////////////////////////////////////////////////////
void RtsGame::InitMap()
{
    auto mapCellSize = g_GameImpl->MapTileSize();

    if (m_isOnline)
    {
        m_pMap = new WorldMap(this, mapCellSize, g_GameImpl->MapWidth() * mapCellSize, g_GameImpl->MapHeight() * mapCellSize);
    }
    else
    {
        // Set the world map grid cell size to be a square of size 8 build tiles
        m_pMap = new WorldMap(this, mapCellSize, m_cachedWorldWidth, m_cachedWorldHeight);
    }
    m_pMap->Init();
}
//////////////////////////////////////////////////////////////////////////
void RtsGame::InitPlayers()
{
    vector<GamePlayer*> oldPlayers;
    EntityList gamePlayerIds;

    m_players.Values(oldPlayers);

    Toolbox::MemoryClean(oldPlayers);
    m_players.clear();

    GamePlayer* pPlayer = nullptr;

    pPlayer = new GamePlayer(g_GameImpl->SelfPlayer());
    pPlayer->Init();
    m_players[PLAYER_Self] = pPlayer;

    pPlayer = new GamePlayer(g_GameImpl->EnemyPlayer());
    pPlayer->Init();
    m_players[PLAYER_Enemy] = pPlayer;

    pPlayer = new GamePlayer(g_GameImpl->NeutralPlayer());
    pPlayer->Init();
    m_players[PLAYER_Neutral] = pPlayer;
}
//////////////////////////////////////////////////////////////////////////
bool RtsGame::InitStaticData()
{
    if (sm_pGameStatics != nullptr)
    {
        LogInfo("RtsGame static data is already initialized");
        return true;
    }

    sm_pGameStatics = new RtsGameStaticData;

    if (g_GameImpl && g_GameImpl->IsInGame())
    {
        LogInfo("Loading game static data from current Broodwar game instance revision %d ...", g_GameImpl->ClientVersion());

        InitEntityTypes();
        InitResearchTypes();
        InitRaceTypes();
    }
    else
    {
        LogInfo("Loading game static data from disk '%s' ...", GAME_STATIC_DATA_FILENAME);

        ifstream f(GAME_STATIC_DATA_FILENAME);

        if (f.is_open())
        {
            f.close();
            g_ObjectSerializer.Deserialize(sm_pGameStatics, GAME_STATIC_DATA_FILENAME);
        }
        else
        {
            LogError("Failed to load game static data from disk. '%s does not exist or access denied");
            return false;
        }
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////
void RtsGame::InitEntityTypes()
{
    vector<IStrategizer::GameType*> oldEntityTypes;

    sm_pGameStatics->EntityTypes.Values(oldEntityTypes);
    Toolbox::MemoryClean(oldEntityTypes);
    sm_pGameStatics->EntityTypes.clear();

    auto gameTypes = g_GameImpl->GetUnitTypes();
    GameType* pType = nullptr;

    for (int i = 0; i < gameTypes->Size(); ++i)
    {
        auto pGameType = gameTypes->At(i);
        pType = new GameType(pGameType);
        pType->Init();
        sm_pGameStatics->EntityTypes[pGameType->EngineId()] = pType;
    }
}
//////////////////////////////////////////////////////////////////////////
void RtsGame::InitResearchTypes()
{
    vector<GameResearch*> oldResearchTypes;

    sm_pGameStatics->ResearchTypes.Values(oldResearchTypes);
    Toolbox::MemoryClean(oldResearchTypes);
    sm_pGameStatics->ResearchTypes.clear();

    auto gameTechTypes = g_GameImpl->GetTechTypes();
    auto gameUpgradeTypes = g_GameImpl->GetUpgradeTypes();
    GameResearch* pType = nullptr;

    for (int i = 0; i < gameTechTypes->Size(); ++i)
    {
        auto pResearchType = gameTechTypes->At(i);
        pType = new GameResearch(pResearchType);;
        pType->Init();
        sm_pGameStatics->ResearchTypes[pResearchType->EngineId()] = pType;
    }

    for (int i = 0; i < gameUpgradeTypes->Size(); ++i)
    {
        auto pUpgradeType = gameUpgradeTypes->At(i);
        pType = new GameResearch(pUpgradeType);
        pType->Init();
        sm_pGameStatics->ResearchTypes[pUpgradeType->EngineId()] = pType;
    }
}
//////////////////////////////////////////////////////////////////////////
void RtsGame::InitRaceTypes()
{
    auto races = g_GameImpl->GetRaces();
    for (int i = 0; i < races->Size(); ++i)
    {
        auto pRace = races->At(i);
        sm_pGameStatics->RaceTypes[pRace->GameId()] = new GameRace(pRace);
    }
}
//////////////////////////////////////////////////////////////////////////
int RtsGame::GameFrame() const
{
    if (m_isOnline)
        return g_GameImpl->GameFrame();
    else
        return m_cachedGameFrame;
}
//////////////////////////////////////////////////////////////////////////
void RtsGame::DebugDrawMapLine(_In_ Vector2 p1, _In_ Vector2 p2, _In_ GameDrawColor c)
{
    if (p1.IsInf() || p2.IsInf())
        return;

    g_GameImpl->DebugDrawMapLine(p1, p2, c);
}
//////////////////////////////////////////////////////////////////////////
void RtsGame::DebugDrawMapCircle(_In_ Vector2 p, _In_ int r, _In_ GameDrawColor c, _In_ bool fill)
{
    if (p.IsInf() || r <= 0)
        return;

    g_GameImpl->DebugDrawMapCircle(p, r, c, fill);
}
//////////////////////////////////////////////////////////////////////////
void RtsGame::DebugDrawMapText(_In_ Vector2 p, _In_ const char* pTxt)
{
    if (p.IsInf())
        return;

    g_GameImpl->DebugDrawMapText(p, pTxt);
}
//////////////////////////////////////////////////////////////////////////
void RtsGame::DebugDrawMapRectangle(_In_ Vector2 topLeft, _In_ Vector2 bottomRight, _In_ GameDrawColor c, _In_ bool fill)
{
    if (topLeft.IsInf() || bottomRight.IsInf())
        return;

    g_GameImpl->DebugDrawMapRectangle(topLeft, bottomRight, c, fill);
}
//////////////////////////////////////////////////////////////////////////
void RtsGame::DebugDrawScreenText(_In_ Vector2 p, _In_ const char* pTxt, _In_ GameDrawColor c)
{
    if (p.IsInf())
        return;

    g_GameImpl->DebugDrawScreenText(p, pTxt, c);
}