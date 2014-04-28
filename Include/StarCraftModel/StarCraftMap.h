#ifndef STARCRAFTMAP_H
#define STARCRAFTMAP_H

#include "WorldMap.h"
#include "TypesMetaData.h"
#include "MapArea.h"

namespace StarCraftModel
{
    class StarCraftMap : public IStrategizer::WorldMap
    {
    public:
        StarCraftMap(unsigned cellSize) : WorldMap(cellSize, cellSize, Size().X, Size().Y) { }
        IStrategizer::Vector2 Size() const;
        bool CanBuildHere(IStrategizer::Vector2 p_position, IStrategizer::EntityClassType p_buildingType) const;
        IStrategizer::MapArea GetSpecialBuildingPosition(IStrategizer::EntityClassType p_buildingType) const;
    };
}

#endif // STARCRAFTMAP_H
