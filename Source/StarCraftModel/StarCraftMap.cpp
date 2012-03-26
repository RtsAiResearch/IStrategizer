#ifndef STARCRAFTMAP_H
#include "StarCraftMap.h"
#endif

#include "bwapi/Game.h"
#include "bwapi/Region.h"
#include "bwapi/Constants.h"

using namespace StarCraftModel;
using namespace BWAPI;

Vector2 StarCraftMap::Size() const
{
    return Vector2(Broodwar->mapWidth() * TILE_SIZE, Broodwar->mapHeight() * TILE_SIZE);
}
////----------------------------------------------------------------------------------------------
//bool StarCraftMap::IsBuildable(Vector2 p_position, Vector2 p_dimension)
//{
//	Region pRegion = Broodwar->getRegionAt(p_position.X, p_position.Y);
//	assert(pRegion);
//}