#ifndef WARGUSPLAYERRESOURCES_H
#include "StarCraftPlayerResources.h"
#endif

#include "BWAPI.h"
#include "DefinitionCrossMapping.h"

using namespace StarCraftModel;
using namespace BWAPI;

int StarCraftPlayerResources::Supply() 
{ 
    return m_player->supplyTotal() - m_player->supplyUsed();
}
//////////////////////////////////////////////////////////////////////////
int StarCraftPlayerResources::Secondary()
{
    return m_player->gas();
}
//////////////////////////////////////////////////////////////////////////
int StarCraftPlayerResources::Primary()
{
    return m_player->minerals();
}

IStrategizer::EntityClassType StarCraftModel::StarCraftPlayerResources::GetEntityClassType( ResourceType p_type ) const
{
    switch(p_type)
    {
    case RESOURCE_Primary:
        return g_Database.EntityMapping.GetByFirst(UnitTypes::Resource_Mineral_Field.getID());
    case RESOURCE_Secondary:
        return g_Database.EntityMapping.GetByFirst(UnitTypes::Resource_Vespene_Geyser.getID());
    }
}
