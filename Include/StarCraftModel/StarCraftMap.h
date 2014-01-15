#ifndef STARCRAFTMAP_H
#define STARCRAFTMAP_H

#ifndef WORLDMAP_H
#include "WorldMap.h"
#endif

namespace StarCraftModel
{
    class StarCraftMap : public IStrategizer::WorldMap
    {
    public:
        StarCraftMap(unsigned cellSize) 
            : WorldMap(cellSize, cellSize, Size().X, Size().Y) 
        { }

        IStrategizer::Vector2 Size() const;
        bool IsBuildable(IStrategizer::Vector2 p_position, IStrategizer::Vector2 p_dimension) const ;
    };
}

#endif // STARCRAFTMAP_H
