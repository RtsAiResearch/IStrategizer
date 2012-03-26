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
		StarCraftMap() : WorldMap(8,8) {}
		Vector2	Size() const;
		bool	IsBuildable(Vector2 p_position, Vector2 p_dimension) const ;
    };
}

#endif // STARCRAFTMAP_H