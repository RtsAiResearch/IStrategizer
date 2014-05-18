#ifndef WARGUSPLAYERRESOURCES_H
#include "StarCraftPlayerResources.h"
#endif

#include "BWAPI.h"
#include "DefinitionCrossMapping.h"
#include "IStrategizerException.h"
#include "ObjectFactory.h"

using namespace IStrategizer;
using namespace BWAPI;

DECL_SERIALIZABLE(StarCraftPlayerResources);

int StarCraftPlayerResources::Supply() 
{
    if (m_isOnline)
    {
        // The amount of supply is doubled because of an issue with the Zerg supply
        // that's why we divide the supplyTotal over 2.
        // For more info check documentation for supplyTotal API.
        int bwapiTotalSupply = m_player->supplyTotal();
        int bwapiUsedSupply = m_player->supplyUsed();
        int totalSupply = bwapiTotalSupply / 2;
        int usedSupply = (bwapiUsedSupply / 2);
        return totalSupply - usedSupply;
    }
    else
        return m_cachedSupply;
}
//////////////////////////////////////////////////////////////////////////
int StarCraftPlayerResources::Secondary()
{
    if (m_isOnline)
        return m_player->gas();
    else
        return m_cachedSecondary;
}
//////////////////////////////////////////////////////////////////////////
int StarCraftPlayerResources::Primary()
{
    if (m_isOnline)
        return m_player->minerals();
    else
        return m_cachedPrimary;
}

