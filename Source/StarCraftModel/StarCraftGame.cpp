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

#include "bwapi/Game.h"
#include "bwapi/Player.h"
#include "bwapi/UnitType.h"
#include "bwapi/Unit.h"
#include "bwapi/TechType.h"
#include "bwapi/UpgradeType.h"

using namespace StarCraftModel;
using namespace IStrategizer;
using namespace MetaData;
using namespace BWAPI;
using namespace std;

StarCraftGame::StarCraftGame()
{
	g_Database.Init();
}
//----------------------------------------------------------------------------------------------
void StarCraftGame::InitializeMap()
{
    m_pMap = new StarCraftMap();
}
//----------------------------------------------------------------------------------------------
void StarCraftGame::EnumeratePlayers()
{
    vector<GamePlayer*>		oldPlayers;
	vector<TID>				gamePlayerIds;
	MetaData::PlayerType	typeId;

    m_players.Values(oldPlayers);

    Toolbox::MemoryClean(oldPlayers);
    m_players.clear();
    
	g_Database.PlayerMapping.FirstValues(gamePlayerIds);

	for (int i = 0, size = gamePlayerIds.size(); i < size; ++i)
	{
		typeId = g_Database.PlayerMapping.GetByFirst(gamePlayerIds[i]);
		m_players[typeId] = NULL;
	}
}
//----------------------------------------------------------------------------------------------
void StarCraftGame::EnumerateEntityTypes()
{
	vector<IStrategizer::GameType*>		oldEntityTypes;
	vector<EntityClassType> newEntityTypes;
	
    m_entityTypes.Values(oldEntityTypes);
    Toolbox::MemoryClean(oldEntityTypes);
    m_entityTypes.clear();

	g_Database.EntityTypes(newEntityTypes);

    for(unsigned i = 0, size = newEntityTypes.size(); i < size; ++i)
    {
        m_entityTypes[(EntityClassType)newEntityTypes[i]] = NULL;
    }
}
//----------------------------------------------------------------------------------------------
void StarCraftGame::EnumerateResearches()
{
    vector<GameResearch*>	oldResearchTypes;
	vector<ResearchType>	newResearchTypes;

    m_researches.Values(oldResearchTypes);
    Toolbox::MemoryClean(oldResearchTypes);
    m_researches.clear();

	g_Database.ResearchTypes(newResearchTypes);

    for(unsigned i = 0, size = newResearchTypes.size(); i < size; ++i)
    {
        m_researches[(ResearchType)newResearchTypes[i]] = NULL;
    }
}
//----------------------------------------------------------------------------------------------
GamePlayer* StarCraftGame::FetchPlayer(MetaData::PlayerType p_id)
{
    TID typeId = g_Database.PlayerMapping.GetBySecond(p_id);
	BWAPI::Player* pPlayer = Broodwar->getPlayer(typeId);

    return new StarCraftPlayer(pPlayer);
}
//----------------------------------------------------------------------------------------------
IStrategizer::GameType* StarCraftGame::FetchEntityType(EntityClassType p_id)
{
	TID						unitId;
    string					typeIdent; 
	BWAPI::UnitType			unitType;
	IStrategizer::GameType*	entityType;

	unitId = g_Database.EntityMapping.GetBySecond(p_id);
	typeIdent = g_Database.EntityIdentMapping.GetByFirst(unitId);

	unitType = UnitTypes::getUnitType(typeIdent);
	entityType = new StarCraftType(unitType);
    entityType->Initialize();

    return entityType;
}
//----------------------------------------------------------------------------------------------
GameResearch* StarCraftGame::FetchResearch(ResearchType p_id)
{
	TID			researchId;
    string				typeIdent;
	BWAPI::UpgradeType	upgrade;
	TechType			tech;
	GameResearch*		research;
	
	researchId = g_Database.UpgradeMapping.GetBySecond(p_id);
	typeIdent = g_Database.UpgradeIdentMapping.GetByFirst(p_id);

    if(p_id >= TechIdOffset)
    {
		tech = TechTypes::getTechType(typeIdent);
		research = new StarCraftResearch(tech);
    }
    else
    {
		upgrade = UpgradeTypes::getUpgradeType(typeIdent);
		research = new StarCraftResearch(upgrade);
    }

	assert(research);
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

	unsigned	cmdLen;
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
    Broodwar->sendText(p_msg);
}