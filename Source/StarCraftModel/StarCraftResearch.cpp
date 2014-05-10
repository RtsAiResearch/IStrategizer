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

using namespace StarCraftModel;
using namespace BWAPI;

void StarCraftResearch::InitializeAux()
{
    if (m_isTech)
    {
        m_id = g_Database.TechMapping.GetByFirst(m_tech.getID());
        m_requiredResources = new WorldResources(0, m_tech.gasPrice(), m_tech.mineralPrice());
    }
    else
    {
        m_id = g_Database.UpgradeMapping.GetByFirst(m_upgrade.getID());
        m_requiredResources = new WorldResources(0, m_upgrade.gasPrice(), m_upgrade.mineralPrice());
    }
}
