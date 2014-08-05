#ifndef STARCRAFTGAME_H
#include "StarCraftGame.h"
#endif
#ifndef GAMEPLAYER_H
#include "GamePlayer.h"
#endif
#ifndef STARCRAFTMAP_H
#include "StarCraftMap.h"
#endif
#ifndef STARCRAFTENTITY_H
#include "StarCraftEntity.h"
#endif
#ifndef METADATA_H
#include "MetaData.h"
#endif
#ifndef DEFINITIONCROSSMAPPING_H
#include "DefinitionCrossMapping.h"
#endif
#ifndef STARCRAFTPLAYER_H
#include "StarCraftPlayer.h"
#endif
#ifndef WORLDRESOURCES_H
#include "WorldResources.h"
#endif
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif
#ifndef STARCRAFTTYPE_H
#include "StarCraftType.h"
#endif
#ifndef STARCRAFTRESEARCH_H
#include "StarCraftResearch.h"
#endif
#include "StarCraftRace.h"
#include "ObjectFactory.h"
#include "ObjectSerializer.h"
#include "BWAPI.h"
#include "IStrategizer.h"

using namespace IStrategizer;
using namespace BWAPI;
using namespace Serialization;
using namespace std;

DECL_SERIALIZABLE(StarCraftGame);

//----------------------------------------------------------------------------------------------
void StarCraftGame::InitMap()
{
    if (m_isOnline)
    {
        // Set the world map grid cell size to be a square of size 8 build tiles
        m_pMap = new StarCraftMap(this, TILE_SIZE * 8, Broodwar->mapWidth() * TILE_SIZE, Broodwar->mapHeight() * TILE_SIZE);
    }
    else
    {
        // Set the world map grid cell size to be a square of size 8 build tiles
        m_pMap = new StarCraftMap(this, TILE_SIZE * 8, m_cachedWorldWidth, m_cachedWorldHeight);
    }
    m_pMap->Init();
}
//----------------------------------------------------------------------------------------------
void StarCraftGame::InitPlayers()
{
    vector<GamePlayer*> oldPlayers;
    EntityList gamePlayerIds;
    IStrategizer::PlayerType typeId;

    m_players.Values(oldPlayers);

    Toolbox::MemoryClean(oldPlayers);
    m_players.clear();

    g_Database.PlayerMapping.FirstValues(gamePlayerIds);

    for (int i = 0, size = gamePlayerIds.size(); i < size; ++i)
    {
        typeId = g_Database.PlayerMapping.GetByFirst(gamePlayerIds[i]);
        m_players[typeId] = FetchPlayer(typeId);
        m_players[typeId]->Init();
    }
}
//----------------------------------------------------------------------------------------------
bool StarCraftGame::InitStaticData()
{
    if (sm_pGameStatics != nullptr)
    {
        LogInfo("RtsGame static data is already initialized");
        return true;
    }

    sm_pGameStatics = new RtsGameStaticData;

    if (BroodwarPtr && Broodwar->isInGame())
    {
        LogInfo("Loading game static data from current Broodwar game instance revision %d ...", Broodwar->getRevision());

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
//----------------------------------------------------------------------------------------------
void StarCraftGame::InitEntityTypes()
{
    vector<IStrategizer::GameType*> oldEntityTypes;
    vector<EntityClassType> newEntityTypes;

    sm_pGameStatics->EntityTypes.Values(oldEntityTypes);
    Toolbox::MemoryClean(oldEntityTypes);
    sm_pGameStatics->EntityTypes.clear();

    g_Database.EntityTypes(newEntityTypes);

    for (unsigned i = 0, size = newEntityTypes.size(); i < size; ++i)
    {
        sm_pGameStatics->EntityTypes[newEntityTypes[i]] = FetchEntityType(newEntityTypes[i]);
    }

}
//----------------------------------------------------------------------------------------------
void StarCraftGame::InitResearchTypes()
{
    vector<GameResearch*> oldResearchTypes;
    vector<ResearchType> newResearchTypes;

    sm_pGameStatics->ResearchTypes.Values(oldResearchTypes);
    Toolbox::MemoryClean(oldResearchTypes);
    sm_pGameStatics->ResearchTypes.clear();

    g_Database.ResearchTypes(newResearchTypes);

    for (unsigned i = 0, size = newResearchTypes.size(); i < size; ++i)
    {
        sm_pGameStatics->ResearchTypes[newResearchTypes[i]] = FetchResearch(newResearchTypes[i]);
    }
}
//----------------------------------------------------------------------------------------------
GamePlayer* StarCraftGame::FetchPlayer(IStrategizer::PlayerType p_id)
{
    BWAPI::Player pPlayer;

    if (p_id == PLAYER_Neutral)
        pPlayer = Broodwar->neutral();
    else
    {
        TID typeId = g_Database.PlayerMapping.GetBySecond(p_id);
        pPlayer = Broodwar->getPlayer(typeId);
    }

    return new StarCraftPlayer(pPlayer);
}
//----------------------------------------------------------------------------------------------
IStrategizer::GameType* StarCraftGame::FetchEntityType(EntityClassType p_id)
{
    TID unitId;
    string typeIdent;
    BWAPI::UnitType unitType;
    IStrategizer::GameType* entityType;

    unitId = g_Database.EntityMapping.GetBySecond(p_id);
    typeIdent = g_Database.EntityIdentMapping.GetByFirst(unitId);

    unitType = UnitType::getType(typeIdent);
    entityType = new StarCraftType(unitType);
    entityType->Init();

    return entityType;
}
//----------------------------------------------------------------------------------------------
GameResearch* StarCraftGame::FetchResearch(ResearchType p_id)
{
    TID researchId;
    string typeIdent;
    BWAPI::UpgradeType upgrade;
    TechType tech;
    GameResearch* research;

    if (p_id >= ((int)(START(ResearchType) + TechIdOffset)))
    {
        researchId = g_Database.TechMapping.GetBySecond(p_id);
        typeIdent = g_Database.TechIdentMapping.GetByFirst(researchId);
        tech = TechType::getType(typeIdent);
        research = new StarCraftResearch(tech);
    }
    else
    {
        researchId = g_Database.UpgradeMapping.GetBySecond(p_id);
        typeIdent = g_Database.UpgradeIdentMapping.GetByFirst(researchId);
        upgrade = UpgradeType::getType(typeIdent);
        research = new StarCraftResearch(upgrade);
    }

    _ASSERTE(research);
    research->Init();

    return research;
}
//----------------------------------------------------------------------------------------------
void StarCraftGame::InitRaceTypes()
{
    for (Race& race : Races::allRaces())
    {
        sm_pGameStatics->RaceTypes[race.getID()] = new StarCraftRace(race);
    }
}
//----------------------------------------------------------------------------------------------
void StarCraftGame::ExecuteCommand(const char *p_cmd)
{
    static const char* commands[] = {
        "export-all-ids",
        "export-game-ids",
        "export-statics"
    };

    unsigned cmdLen;
    const char* cmdParam;

    if (!strncmp(p_cmd, commands[0], strlen(commands[0])))
    {
        cmdLen = strlen(commands[0]);
        cmdParam = p_cmd + cmdLen + 1;

        if (g_Database.ExportAllIds(cmdParam))
            DisplayMessage("All ids exported successfully");
        else
            DisplayMessage("Failed to export all ids");
    }
    else if (!strncmp(p_cmd, commands[1], strlen(commands[1])))
    {
        cmdLen = strlen(commands[1]);
        cmdParam = p_cmd + cmdLen + 1;

        if (g_Database.ExportGameIds(cmdParam))
            DisplayMessage("Game ids exported successfully");
        else
            DisplayMessage("Failed to export game ids");
    }

}
//----------------------------------------------------------------------------------------------
void StarCraftGame::DisplayMessage(const char* p_msg)
{
    BroodwarPtr->sendText(p_msg);
}
//----------------------------------------------------------------------------------------------
unsigned StarCraftGame::GameFrame() const
{
    if (m_isOnline)
        return Broodwar->getFrameCount();
    else
        return m_cachedGameFrame;

}
//////////////////////////////////////////////////////////////////////////
BWAPI::Color BwapiColorFrom(GameDrawColor c)
{
    switch (c)
    {
    case GCLR_Red:
        return Colors::Red;
    case GCLR_Green:
        return Colors::Green;
    case GCLR_Blue:
        return Colors::Blue;
    case GCLR_Yellow:
        return Colors::Yellow;
    case GCLR_Orange:
        return Colors::Orange;
    case GCLR_Cyan:
        return Colors::Cyan;
    case GCLR_Purple:
        return Colors::Purple;
    case GCLR_White:
    default:
        return Colors::White;
    }
}
//////////////////////////////////////////////////////////////////////////
void StarCraftGame::DebugDrawMapLine(_In_ Vector2 p1, _In_ Vector2 p2, _In_ GameDrawColor c)
{
    if (p1.IsInf() || p2.IsInf())
        return;

    Broodwar->drawLineMap(p1.X, p1.Y, p2.X, p2.Y, BwapiColorFrom(c));
}
//////////////////////////////////////////////////////////////////////////
void StarCraftGame::DebugDrawMapCircle(_In_ Vector2 p, _In_ int r, _In_ GameDrawColor c, _In_ bool fill)
{
    if (p.IsInf() || r <= 0)
        return;

    Broodwar->drawCircleMap(p.X, p.Y, r, BwapiColorFrom(c), fill);
}
//////////////////////////////////////////////////////////////////////////
void StarCraftGame::DebugDrawMapText(_In_ Vector2 p, _In_ const std::string& txt)
{
    if (p.IsInf())
        return;

    Broodwar->drawTextMap(p.X, p.Y, txt.c_str());
}
//////////////////////////////////////////////////////////////////////////
void StarCraftGame::DebugDrawMapRectangle(_In_ Vector2 topLeft, _In_ Vector2 bottomRight, _In_ GameDrawColor c, _In_ bool fill)
{
    if (topLeft.IsInf() || bottomRight.IsInf())
        return;

    Broodwar->drawBoxMap(topLeft.X, topLeft.Y, bottomRight.X, bottomRight.Y, BwapiColorFrom(c), fill);
}
//////////////////////////////////////////////////////////////////////////
void StarCraftGame::DebugDrawScreenText(_In_ Vector2 p, _In_ const std::string& txt, _In_ GameDrawColor c)
{
    if (p.IsInf())
        return;

    Broodwar->drawTextScreen(p.X, p.Y, txt.c_str());
}