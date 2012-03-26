#ifndef STARCRAFTTECHTREE_H
#include "StarCraftTechTree.h"
#endif
#ifndef METADATA_H
#include "MetaData.h"
#endif
#ifndef DEFINITIONCROSSMAPPING_H
#include "DefinitionCrossMapping.h"
#endif
#ifndef MAPEX_H
#include "MapEx.h"
#endif
#include <cassert>
#include <string>
#include <map>
#include "bwapi/Game.h"
#include "bwapi/TechType.h"
#include "bwapi/UpgradeType.h"
#include "bwapi/UnitType.h"

using namespace IStrategizer;
using namespace StarCraftModel;
using namespace MetaData;
using namespace DataStructure;
using namespace BWAPI;
using namespace std;

//----------------------------------------------------------------------------------------------
bool StarCraftTechTree::ResearchAvailable(ResearchType p_researchId) const
{
	TID	researchId;
	TName		researchIdent;
	TechType	bwapiTech;
	UpgradeType bwapiUpgrade;

	if (p_researchId == RESEARCH_END)
		return false;

	// Is Tech
	if ((int)p_researchId >= TechIdOffset)
	{
		researchId = g_Database.TechMapping.GetBySecond(p_researchId);
		researchIdent = g_Database.TechIdentMapping.GetByFirst(researchId);
		bwapiTech = TechTypes::getTechType(researchIdent);

		return m_player->isResearchAvailable(bwapiTech);
	}
	// Is Upgrade
	else
	{
		researchId = g_Database.UpgradeMapping.GetBySecond(p_researchId);
		researchIdent = g_Database.UpgradeIdentMapping.GetByFirst(researchId);
		bwapiUpgrade = UpgradeTypes::getUpgradeType(researchIdent);

		return (m_player->getMaxUpgradeLevel(bwapiUpgrade) > 0);
	}
}   
//----------------------------------------------------------------------------------------------
bool StarCraftTechTree::ResearchDone(ResearchType p_researchId) const
{
	TID	researchId;
	TName		researchIdent;
	TechType	tech;
	UpgradeType upgrade;

	if (p_researchId == RESEARCH_END)
		return false;

	// Is Tech
	if ((int)p_researchId >= TechIdOffset)
	{
		researchId = g_Database.TechMapping.GetBySecond(p_researchId);
		researchIdent = g_Database.TechIdentMapping.GetByFirst(researchId);
		tech = TechTypes::getTechType(researchIdent);

		return m_player->hasResearched(tech);
	}
	// Is Upgrade
	else
	{
		// FIXME: we restrict upgrade levels to be 1 level only
		return m_player->getUpgradeLevel(upgrade) > 0;
	}
}   
//----------------------------------------------------------------------------------------------
void StarCraftTechTree::GetRequirements(int p_typeOrResearchId, vector<ResearchType>& p_researches, map<EntityClassType, unsigned>& p_buildings)
{
	TName			ident;
	TID		id;
	UpgradeType		bwapiUpgrade;
	TechType		bwapiTech;
	BWAPI::UnitType bwapiUnitType;
	BWAPI::UnitType	bwapiSourceType;
	BWAPI::UnitType bwapiRequiredType;
	BWAPI::UnitType	bwapiRequiredUnit;
	TechType		bwapiRequiredTech;
	EntityClassType requiredEntity;
	ResearchType	requiredResearch;
	
	if (BELONG(ResearchType, p_typeOrResearchId))
	{
		// Is Tech
		if ((int)p_typeOrResearchId >= TechIdOffset)
		{
			/*id = g_Database.TechMapping.GetBySecond((ResearchType)p_typeOrResearchId);
			ident = g_Database.TechIdentMapping.GetByFirst(id);
			bwapiTech = TechTypes::getTechType(ident);

			bwapiSourceType = bwapiTech.whatResearches();
			requiredEntity = g_Database.EntityMapping.GetByFirst(bwapiSourceType.getID());
			p_buildings.push_back(requiredEntity);*/
		}
		// Is Upgrade
		else
		{
			id = g_Database.UpgradeMapping.GetBySecond((ResearchType)p_typeOrResearchId);
			ident = g_Database.UpgradeIdentMapping.GetByFirst(id);
			bwapiUpgrade = UpgradeTypes::getUpgradeType(ident);

			/*bwapiSourceType = bwapiUpgrade.whatUpgrades();
			requiredEntity = g_Database.EntityMapping.GetByFirst(bwapiSourceType.getID());
			p_buildings.push_back(requiredEntity);*/

			bwapiRequiredType = bwapiUpgrade.whatsRequired();

			if (bwapiRequiredType.getID() != UnitTypes::None.getID())
			{
				requiredEntity = g_Database.EntityMapping.GetByFirst(bwapiRequiredType.getID());
				p_buildings[requiredEntity] = 1;
			}
		}
	}
	else if(BELONG(EntityClassType, p_typeOrResearchId))
	{
		id = g_Database.EntityMapping.GetBySecond((EntityClassType)p_typeOrResearchId);
		ident = g_Database.EntityIdentMapping.GetByFirst(id);
		bwapiUnitType = UnitTypes::getUnitType(ident);

		/*bwapiSourceType = bwapiUnitType.whatBuilds().first;
		requiredEntity = g_Database.EntityMapping.GetByFirst(bwapiSourceType.getID());
		p_buildings.push_back(requiredEntity);*/

		bwapiRequiredTech = bwapiUnitType.requiredTech();

		if (bwapiRequiredTech.getID() != TechTypes::None.getID())
		{
			requiredResearch = g_Database.TechMapping.GetByFirst(bwapiRequiredTech.getID());
			p_researches.push_back(requiredResearch);
		}

		const map<BWAPI::UnitType, int> &bwapiUnits = bwapiUnitType.requiredUnits();

		for (map<BWAPI::UnitType, int>::const_iterator itr =  bwapiUnits.begin();
			itr != bwapiUnits.end(); ++itr)
		{
			bwapiRequiredUnit = itr->first;
			requiredEntity = g_Database.EntityMapping.GetByFirst(bwapiRequiredUnit.getID());
			p_buildings[requiredEntity] = itr->second;;
		}
	}
}
//----------------------------------------------------------------------------------------------
void StarCraftTechTree::GetDependents(int p_typeOrResearchId, vector<ResearchType>& p_researches, vector<EntityClassType>& p_entityTypes)
{
	assert(0);
    /*if(m_dependentsCache.Contains(p_typeOrResearchId))
    {
        Dependency& dependents = m_dependentsCache[p_typeOrResearchId];
        p_researches    = dependents.second;
        p_entityTypes   = dependents.first;
    }
    else
    {
        vector<CUnitType*>  gameTypes;
        vector<CUpgrade*>   gameResearches;
        std::string         ident;

        if(BELONG(ResearchType, p_typeOrResearchId))
            ident = g_Database.UpgradeMapping.GetBySecond((ResearchType)p_typeOrResearchId);
        else if(BELONG(EntityClassType, p_typeOrResearchId))
            ident = g_Database.EntityMapping.GetBySecond((EntityClassType)p_typeOrResearchId);

        if(p_typeOrResearchId == RESEARCH_Stronghold || p_typeOrResearchId == RESEARCH_Fortress)
        {
            p_researches.push_back((ResearchType)p_typeOrResearchId);
        }
        else if(GetDependentsAux(m_player, ident, gameResearches, gameTypes))
        {
            p_researches.resize(gameResearches.size());
            for(int i = 0, size = gameResearches.size(); i < size; ++i)
                p_researches[i] = g_Database.UpgradeMapping.GetByFirst(gameResearches[i]->Ident);

            p_entityTypes.resize(gameTypes.size());
            for(int i = 0, size = gameTypes.size(); i < size; ++i)
                p_entityTypes[i] = g_Database.EntityMapping.GetByFirst(gameTypes[i]->Ident);
        }

        Dependency& dependents = m_dependentsCache[p_typeOrResearchId];
        dependents.first  = p_entityTypes;
        dependents.second = p_researches;
    }*/
}
//----------------------------------------------------------------------------------------------
EntityClassType StarCraftTechTree::TireBaseBuilding(BaseType p_tireId) const
{
	BWAPI::UnitType baseType;
	
	if (p_tireId == BASETYPE_END)
		return ECLASS_END;

	baseType = m_player->getRace().getCenter();

	return g_Database.EntityMapping.GetByFirst(baseType.getID());
}
//----------------------------------------------------------------------------------------------
EntityClassType StarCraftTechTree::SourceEntity(int p_typeOrResearchId) const
{
    TName			ident;
	TID				id;
	UpgradeType		upgrade;
	TechType		tech;
	BWAPI::UnitType unitType;
	BWAPI::UnitType	sourceType;

    if (BELONG(ResearchType, p_typeOrResearchId))
    {
		// Is Tech
		if ((int)p_typeOrResearchId >= ((int)(RESEARCH_START +  TechIdOffset)))
		{
			id = g_Database.TechMapping.GetBySecond((ResearchType)p_typeOrResearchId);
			ident = g_Database.TechIdentMapping.GetByFirst(id);
			tech = TechTypes::getTechType(ident);
			sourceType = tech.whatResearches();

			return g_Database.EntityMapping.GetByFirst(sourceType.getID());
		}
		// Is Upgrade
		else
		{
			id = g_Database.UpgradeMapping.GetBySecond((ResearchType)p_typeOrResearchId);
			ident = g_Database.UpgradeIdentMapping.GetByFirst(id);
			upgrade = UpgradeTypes::getUpgradeType(ident);
			sourceType = upgrade.whatUpgrades();

			return g_Database.EntityMapping.GetByFirst(sourceType.getID());
		}
    }
    else if(BELONG(EntityClassType, p_typeOrResearchId))
    {
        id = g_Database.EntityMapping.GetBySecond((EntityClassType)p_typeOrResearchId);
		ident = g_Database.EntityIdentMapping.GetByFirst(id);
		unitType = UnitTypes::getUnitType(ident);
		sourceType = unitType.whatBuilds().first;

        return g_Database.EntityMapping.GetByFirst(sourceType.getID());
    }

    return ECLASS_END;
}
