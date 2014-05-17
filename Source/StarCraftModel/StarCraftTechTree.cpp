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

using namespace IStrategizer;
using namespace BWAPI;
using namespace std;

StarCraftTechTree::StarCraftTechTree(BWAPI::Player pPlayer) :
    m_pPlayer(pPlayer)
{

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
