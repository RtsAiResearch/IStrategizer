#ifndef STARCRAFTMAP_H
#define STARCRAFTMAP_H

#ifndef WORLDMAP_H
#include "WorldMap.h"
#endif

namespace BWAPI
{
	class Game;
}

namespace StarCraftModel
{
    using namespace IStrategizer;
    class StarCraftMap : public WorldMap
    {
    public:
		StarCraftMap() : WorldMap(128, 128, Size().X, Size().Y) { }
		Vector2	Size() const;
		bool	IsBuildable(Vector2 p_position, Vector2 p_dimension) const ;
    };
}

#endif // STARCRAFTMAP_H