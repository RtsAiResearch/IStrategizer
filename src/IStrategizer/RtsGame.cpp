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
#include "PlayerResources.h"
#include "ObjectFactory.h"
#include "EngineDefs.h"

using namespace IStrategizer;
using namespace Serialization;
using namespace std;

RtsGame* g_Game = nullptr;
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
RtsGame::~RtsGame()
{
    Finalize();
}
//////////////////////////////////////////////////////////////////////////
void RtsGame::Init()
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
        }
    }

    if (m_isInitialized)
        return;

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
        DEBUG_THROW(ItemNotFoundException(XcptHere));

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
        val = m_game.Self()->CountEntityTypes(ECATTR_IsBuilding, 1);
        break;
    case RTSMODATTR_Player_Entities_NumWorkers:
        val = m_game.Self()->CountEntityTypes(ECATTR_CanBuild, 1);
        break;
    case RTSMODATTR_Player_Entities_NumAttackers:
        val = m_game.Self()->CountEntityTypes(ECATTR_IsAttacker, 1);
        break;
    case RTSMODATTR_Player_Entities_NumDoneResearches:
        m_game.Researches(researches);
        for (auto researchId : researches)
        {
            if (m_game.Self()->TechTree()->ResearchDone(researchId))
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
void RtsGame::Update()
{
    if (m_firstUpdate)
    {
        m_clock.Reset();
        m_firstUpdate = false;
    }

    m_clock.Update(GameFrame());
}
