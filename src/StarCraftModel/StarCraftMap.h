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
        StarCraftMap(RtsGame* pGame, unsigned cellSize, unsigned p_worldWidth, unsigned p_worldHeight);
		bool CanBuildHere(Vector2 p_position, EntityClassType p_buildingType) const;
		bool CanBuildHere(Vector2 pos) const;
        MapArea GetSpecialBuildingPosition(EntityClassType p_buildingType) const;
        void SpawnLocations(_Out_ std::vector<Vector2>& spawnPoints) const;
        bool IsLocationExplored(Vector2 loc) const;
        void DebugDraw();
    };
}

#endif // STARCRAFTMAP_H
