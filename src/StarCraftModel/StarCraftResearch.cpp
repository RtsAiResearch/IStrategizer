#ifndef STARCRAFTRESEARCH_H
#include "StarCraftResearch.h"
#endif
#ifndef DEFINITIONCROSSMAPPING_H
#include "DefinitionCrossMapping.h"
#endif
#ifndef WORLDRESOURCES_H
#include "WorldResources.h"
#endif
#include <cassert>
#include "BWAPI.h"

using namespace IStrategizer;
using namespace BWAPI;
using namespace std;

void StarCraftResearch::Init()
{
    if (m_isTech)
    {
        m_id = g_Database.TechMapping.GetByFirst(m_tech.getID());
        m_pRequiredResources = WorldResources(0, m_tech.gasPrice(), m_tech.mineralPrice());
    }
    else
    {
        m_id = g_Database.UpgradeMapping.GetByFirst(m_upgrade.getID());
        m_pRequiredResources =  WorldResources(0, m_upgrade.gasPrice(), m_upgrade.mineralPrice());
    }
}

void StarCraftResearch::GetRequirements(vector<ResearchType>& researches, map<EntityClassType, unsigned>& buildings) const
{
    // Is Upgrade
    if (!m_isTech)
    {
        if (m_upgrade.whatsRequired().getID() != UnitTypes::None.getID())
        {
            EntityClassType requiredEntity = g_Database.EntityMapping.GetByFirst(m_upgrade.whatsRequired().getID());
            buildings[requiredEntity] = 1;
        }
    }
}

EntityClassType StarCraftResearch::SourceEntity() const
{
    // Is Tech
    if (m_isTech)
    {
        return g_Database.EntityMapping.GetByFirst(m_tech.whatResearches().getID());
    }
    // Is Upgrade
    else
    {
        return g_Database.EntityMapping.GetByFirst(m_upgrade.whatUpgrades().getID());
    }
}