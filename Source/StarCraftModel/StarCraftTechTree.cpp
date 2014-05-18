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
#include "ObjectFactory.h"
#include "RtsGame.h"
#include "BWAPI.h"

using namespace IStrategizer;
using namespace BWAPI;
using namespace std;

DECL_SERIALIZABLE(StarCraftTechTree);

bool StarCraftTechTree::ResearchAvailable(ResearchType researchId) const
{
    if (m_isOnline)
    {
        TID researchGameId;
        TName researchIdent;
        TechType bwapiTech;
        UpgradeType bwapiUpgrade;

        if (researchId == RESEARCH_END)
            return false;

        // Is Tech
        if ((int)researchId >= ((int)(RESEARCH_START +  TechIdOffset)))
        {
            researchGameId = g_Database.TechMapping.GetBySecond(researchId);
            researchIdent = g_Database.TechIdentMapping.GetByFirst(researchGameId);
            bwapiTech = TechType::getType(researchIdent);

            return m_pPlayer->isResearchAvailable(bwapiTech);
        }
        // Is Upgrade
        else
        {
            researchGameId = g_Database.UpgradeMapping.GetBySecond(researchId);
            researchIdent = g_Database.UpgradeIdentMapping.GetByFirst(researchGameId);
            bwapiUpgrade = UpgradeType::getType(researchIdent);

            return (m_pPlayer->getMaxUpgradeLevel(bwapiUpgrade) > 0);
        }
    }
    else
        return m_cachedAvailResearches.count(researchId) > 0;
}   

bool StarCraftTechTree::ResearchDone(ResearchType researchId) const
{
    if (m_isOnline)
    {
        TID researchGameId;
        TName researchIdent;
        TechType tech;
        UpgradeType upgrade;

        if (researchId == RESEARCH_END)
            return false;

        // Is Tech
        if ((int)researchId >= ((int)(RESEARCH_START +  TechIdOffset)))
        {
            researchGameId = g_Database.TechMapping.GetBySecond(researchId);
            researchIdent = g_Database.TechIdentMapping.GetByFirst(researchGameId);
            tech = TechType::getType(researchIdent);

            return m_pPlayer->hasResearched(tech);
        }
        // Is Upgrade
        else
        {
            researchGameId = g_Database.UpgradeMapping.GetBySecond(researchId);
            researchIdent = g_Database.UpgradeIdentMapping.GetByFirst(researchGameId);
            upgrade = UpgradeType::getType(researchIdent);

            // FIXME: we restrict upgrade levels to be 1 level only
            return m_pPlayer->getUpgradeLevel(upgrade) > 0;
        }
    }
    else
        return m_cachedDoneResearches.count(researchId) > 0;
}   

void StarCraftTechTree::SetOffline(RtsGame* pBelongingGame)
{
    _ASSERTE(m_isOnline);

    for (auto researchEntry : g_Game->ResearchTypes())
    {
        if (ResearchAvailable(researchEntry.first))
            m_cachedAvailResearches.insert(researchEntry.first);

        if (ResearchDone(researchEntry.first))
            m_cachedDoneResearches.insert(researchEntry.first);
    }

    m_isOnline = false;
}
