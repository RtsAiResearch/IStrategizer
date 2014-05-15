#ifndef STARCRAFTTECHTREE_H
#include "StarCraftTechTree.h"
#endif
#ifndef METADATA_H
#include "MetaData.h"
#endif
#ifndef DEFINITIONCROSSMAPPING_H
#include "DefinitionCrossMapping.h"
#endif
#include <cassert>
#include <string>
#include <map>
#include "BWAPI.h"
#include <Windows.h>

using namespace IStrategizer;
using namespace BWAPI;
using namespace std;

// Been determined empirically by analyzing professional games. 
// Refer to paper "Build Order Optimization in StarCraft" page 3
const float StarCraftTechTree::MineralsPerWorkerPerFrame = 0.045f;
const float StarCraftTechTree::GasPerWorkerPerFrame = 0.07f;

StarCraftTechTree::StarCraftTechTree(BWAPI::Player pPlayer) :
    m_pPlayer(pPlayer)
{
    TID typeId;

    typeId = m_pPlayer->getRace().getWorker().getID();
    m_workerTypeId = g_Database.EntityMapping.GetByFirst(typeId);

    typeId = m_pPlayer->getRace().getCenter().getID();
    m_baseTypeId = g_Database.EntityMapping.GetByFirst(typeId);
}
//----------------------------------------------------------------------------------------------
bool StarCraftTechTree::ResearchAvailable(ResearchType p_researchId) const
{
    TID researchId;
    TName researchIdent;
    TechType bwapiTech;
    UpgradeType bwapiUpgrade;

    if (p_researchId == RESEARCH_END)
        return false;

    // Is Tech
    if ((int)p_researchId >= ((int)(RESEARCH_START +  TechIdOffset)))
    {
        researchId = g_Database.TechMapping.GetBySecond(p_researchId);
        researchIdent = g_Database.TechIdentMapping.GetByFirst(researchId);
        bwapiTech = TechType::getType(researchIdent);

        return m_pPlayer->isResearchAvailable(bwapiTech);
    }
    // Is Upgrade
    else
    {
        researchId = g_Database.UpgradeMapping.GetBySecond(p_researchId);
        researchIdent = g_Database.UpgradeIdentMapping.GetByFirst(researchId);
        bwapiUpgrade = UpgradeType::getType(researchIdent);

        return (m_pPlayer->getMaxUpgradeLevel(bwapiUpgrade) > 0);
    }
}   
//----------------------------------------------------------------------------------------------
bool StarCraftTechTree::ResearchDone(ResearchType p_researchId) const
{
    TID researchId;
    TName researchIdent;
    TechType tech;
    UpgradeType upgrade;

    if (p_researchId == RESEARCH_END)
        return false;

    // Is Tech
    if ((int)p_researchId >= ((int)(RESEARCH_START +  TechIdOffset)))
    {
        researchId = g_Database.TechMapping.GetBySecond(p_researchId);
        researchIdent = g_Database.TechIdentMapping.GetByFirst(researchId);
        tech = TechType::getType(researchIdent);

        return m_pPlayer->hasResearched(tech);
    }
    // Is Upgrade
    else
    {
        researchId = g_Database.UpgradeMapping.GetBySecond(p_researchId);
        researchIdent = g_Database.UpgradeIdentMapping.GetByFirst(researchId);
        upgrade = UpgradeType::getType(researchIdent);

        // FIXME: we restrict upgrade levels to be 1 level only
        return m_pPlayer->getUpgradeLevel(upgrade) > 0;
    }
}   
//----------------------------------------------------------------------------------------------
void StarCraftTechTree::GetRequirements(int p_typeOrResearchId, vector<ResearchType>& p_researches, map<EntityClassType, unsigned>& p_buildings)
{
    TName ident;
    TID id;
    UpgradeType bwapiUpgrade;
    TechType bwapiTech;
    BWAPI::UnitType bwapiUnitType;
    BWAPI::UnitType bwapiSourceType;
    BWAPI::UnitType bwapiRequiredType;
    BWAPI::UnitType bwapiRequiredUnit;
    TechType bwapiRequiredTech;
    EntityClassType requiredEntity;
    ResearchType requiredResearch;
    
    if (BELONG(ResearchType, p_typeOrResearchId))
    {
        // Is Tech
        if ((int)p_typeOrResearchId >= ((int)(RESEARCH_START +  TechIdOffset)))
        {
            /*id = g_Database.TechMapping.GetBySecond((ResearchType)p_typeOrResearchId);
            ident = g_Database.TechIdentMapping.GetByFirst(id);
            bwapiTech = TechType::getType(ident);

            bwapiSourceType = bwapiTech.whatResearches();
            requiredEntity = g_Database.EntityMapping.GetByFirst(bwapiSourceType.getID());
            p_buildings.push_back(requiredEntity);*/
        }
        // Is Upgrade
        else
        {
            id = g_Database.UpgradeMapping.GetBySecond((ResearchType)p_typeOrResearchId);
            ident = g_Database.UpgradeIdentMapping.GetByFirst(id);
            bwapiUpgrade = UpgradeType::getType(ident);

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
        bwapiUnitType = UnitType::getType(ident);

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
EntityClassType StarCraftTechTree::TireBaseBuilding(BaseType p_tireId) const
{
    BWAPI::UnitType baseType;
    
    if (p_tireId == BASETYPE_END)
        return ECLASS_END;

    baseType = m_pPlayer->getRace().getCenter();

    return g_Database.EntityMapping.GetByFirst(baseType.getID());
}
//----------------------------------------------------------------------------------------------
EntityClassType StarCraftTechTree::SourceEntity(int p_typeOrResearchId) const
{
    TName ident;
    TID id;
    UpgradeType upgrade;
    TechType tech;
    BWAPI::UnitType unitType;
    BWAPI::UnitType sourceType;

    if (BELONG(ResearchType, p_typeOrResearchId))
    {
        // Is Tech
        if ((int)p_typeOrResearchId >= ((int)(RESEARCH_START +  TechIdOffset)))
        {
            id = g_Database.TechMapping.GetBySecond((ResearchType)p_typeOrResearchId);
            ident = g_Database.TechIdentMapping.GetByFirst(id);
            tech = TechType::getType(ident);
            sourceType = tech.whatResearches();

            return g_Database.EntityMapping.GetByFirst(sourceType.getID());
        }
        // Is Upgrade
        else
        {
            id = g_Database.UpgradeMapping.GetBySecond((ResearchType)p_typeOrResearchId);
            ident = g_Database.UpgradeIdentMapping.GetByFirst(id);
            upgrade = UpgradeType::getType(ident);
            sourceType = upgrade.whatUpgrades();

            return g_Database.EntityMapping.GetByFirst(sourceType.getID());
        }
    }
    else if(BELONG(EntityClassType, p_typeOrResearchId))
    {
        id = g_Database.EntityMapping.GetBySecond((EntityClassType)p_typeOrResearchId);
        ident = g_Database.EntityIdentMapping.GetByFirst(id);
        unitType = UnitType::getType(ident);
        sourceType = unitType.whatBuilds().first;

        return g_Database.EntityMapping.GetByFirst(sourceType.getID());
    }

    return ECLASS_END;
}
//----------------------------------------------------------------------------------------------
EntityClassType StarCraftTechTree::GetBuilderType(EntityClassType p_buildingType) const
{
    UnitType type;
    TID unitTypeId;
    string typeName;

    unitTypeId = g_Database.EntityMapping.GetBySecond(p_buildingType);
    typeName = g_Database.EntityIdentMapping.GetByFirst(unitTypeId);
    type = BWAPI::UnitType::getType(typeName);

    if (type.isAddon())
    {
        return g_Database.EntityMapping.GetByFirst(type.whatBuilds().first.getID());
    }
    else
    {
        _ASSERTE(type.isBuilding());
        return GetWorkerType();
    }
}
int StarCraftTechTree::BaseSupplyAmount() const
{
    return 10;
}
//----------------------------------------------------------------------------------------------
int StarCraftTechTree::SupplyBuildingSupplyAmount() const
{
    return 8;
}
//----------------------------------------------------------------------------------------------
float StarCraftTechTree::GetResourceConsumbtionRatePerWorker(ResourceType p_id) const
{
    switch(p_id)
    {
    case RESOURCE_Primary:
        return MineralsPerWorkerPerFrame;
    case RESOURCE_Secondary:
        return GasPerWorkerPerFrame;
    default:
        throw InvalidParameterException(XcptHere);
    }
}
//----------------------------------------------------------------------------------------------
EntityClassType StarCraftTechTree::GetResourceSource(ResourceType p_type) const
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
