#ifndef STARCRAFTMAP_H
#include "StarCraftMap.h"
#endif

#include "BWAPI.h"

using namespace StarCraftModel;
using namespace IStrategizer;
using namespace BWAPI;

Vector2 StarCraftMap::Size() const
{
    return Vector2(Broodwar->mapWidth() * TILE_SIZE, Broodwar->mapHeight() * TILE_SIZE);
}
//////////////////////////////////////////////////////////////////////////
IClonable* StarCraftMap::Clone()
{
    StarCraftMap* pClone = new StarCraftMap;
    Copy(pClone);

    return pClone;
}
