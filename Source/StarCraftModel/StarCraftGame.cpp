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

#include "BWAPI.h"

using namespace StarCraftModel;
using namespace IStrategizer;
using namespace IStrategizer;
using namespace BWAPI;
using namespace std;

StarCraftGame::StarCraftGame()
{
    g_Database.Init();
}
//----------------------------------------------------------------------------------------------
void StarCraftGame::InitializeMap()
{
    // Set the world map grid cell size to be a square of size 8 build tiles
    m_pMap = new StarCraftMap(TILE_SIZE * 8);
}
//----------------------------------------------------------------------------------------------
void StarCraftGame::EnumeratePlayers()
{
    vector<GamePlayer*> oldPlayers;
    vector<TID> gamePlayerIds;
    IStrategizer::PlayerType typeId;

    m_players.Values(oldPlayers);

    Toolbox::MemoryClean(oldPlayers);
    m_players.clear();

    g_Database.PlayerMapping.FirstValues(gamePlayerIds);

    for (int i = 0, size = gamePlayerIds.size(); i < size; ++i)
    {
        typeId = g_Database.PlayerMapping.GetByFirst(gamePlayerIds[i]);
        m_players[typeId] = nullptr;
    }
}
//----------------------------------------------------------------------------------------------
void StarCraftGame::EnumerateEntityTypes()
{
    vector<IStrategizer::GameType*> oldEntityTypes;
    vector<EntityClassType> newEntityTypes;

    sm_entityTypes.Values(oldEntityTypes);
    Toolbox::MemoryClean(oldEntityTypes);
    sm_entityTypes.clear();

    g_Database.EntityTypes(newEntityTypes);

    for(unsigned i = 0, size = newEntityTypes.size(); i < size; ++i)
    {
        sm_entityTypes[(EntityClassType)newEntityTypes[i]] = nullptr;
    }
}
//----------------------------------------------------------------------------------------------
void StarCraftGame::EnumerateResearches()
{
    vector<GameResearch*> oldResearchTypes;
    vector<ResearchType> newResearchTypes;

    sm_researches.Values(oldResearchTypes);
    Toolbox::MemoryClean(oldResearchTypes);
    sm_researches.clear();

    g_Database.ResearchTypes(newResearchTypes);

    for(unsigned i = 0, size = newResearchTypes.size(); i < size; ++i)
    {
        sm_researches[(ResearchType)newResearchTypes[i]] = nullptr;
    }
}
//----------------------------------------------------------------------------------------------
GamePlayer* StarCraftGame::FetchPlayer(IStrategizer::PlayerType p_id)
{
    TID typeId = g_Database.PlayerMapping.GetBySecond(p_id);
    BWAPI::Player pPlayer = Broodwar->getPlayer(typeId);

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
    entityType->Initialize();

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

    if(p_id >= ((int)(START(ResearchType) +  TechIdOffset)))
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
    research->Initialize();

    return research;
}
//----------------------------------------------------------------------------------------------
void StarCraftGame::ExecuteCommand(const char *p_cmd)
{
    static const char* commands[] = {
        "export-all-ids",
        "export-game-ids"
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
int StarCraftGame::GetMaxForceSize() const
{
    return 12;
}
//----------------------------------------------------------------------------------------------
size_t StarCraftGame::GetMaxTrainingQueueCount() const
{
    return 5;
}
//----------------------------------------------------------------------------------------------
EntityClassType StarCraftGame::GetResourceSource(ResourceType p_type) const
{
    // This method is hard-coded for Terran, ideally it should take race type.

    switch(p_type)
    {
    case RESOURCE_Primary:
        return g_Database.EntityMapping.GetByFirst(UnitTypes::Resource_Mineral_Field.getID());

    case RESOURCE_Secondary:
        return g_Database.EntityMapping.GetByFirst(UnitTypes::Terran_Refinery.getID());
    
    case RESOURCE_Supply:
        return g_Database.EntityMapping.GetByFirst(UnitTypes::Terran_Supply_Depot.getID());
    
    default:
        DEBUG_THROW(InvalidParameterException(XcptHere));
    }
}
//----------------------------------------------------------------------------------------------
int StarCraftGame::BaseSupplyAmount() const
{
    return 10;
}
//----------------------------------------------------------------------------------------------
int StarCraftGame::SupplyBuildingSupplyAmount() const
{
    return 8;
}