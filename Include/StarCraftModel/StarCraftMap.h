#ifndef STARCRAFTMAP_H
#define STARCRAFTMAP_H

#include "WorldMap.h"
#include "TypesMetaData.h"
#include "MapArea.h"

namespace IStrategizer
{
    class StarCraftMap : public WorldMap
    {
    public:
        StarCraftMap(unsigned cellSize);
        bool CanBuildHere(Vector2 p_position, EntityClassType p_buildingType) const;
        MapArea GetSpecialBuildingPosition(EntityClassType p_buildingType) const;
    };
}

#endif // STARCRAFTMAP_H
