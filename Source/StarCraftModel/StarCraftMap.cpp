#include "StarCraftMap.h"
#include "BWAPI.h"
#include "MapArea.h"
#include "GamePlayer.h"

#include <string>
#include "DefinitionCrossMapping.h"
#include "StarCraftEntity.h"

using namespace StarCraftModel;
using namespace IStrategizer;
using namespace BWAPI;
using namespace std;

Vector2 StarCraftMap::Size() const
{
    return Vector2(Broodwar->mapWidth() * TILE_SIZE, Broodwar->mapHeight() * TILE_SIZE);
}
//----------------------------------------------------------------------------------------------
MapArea StarCraftMap::GetSpecialBuildingPosition(EntityClassType p_buildingType) const
{
    MapArea candidatePosition = MapArea::Null();

    // Get the player base tile position
    MapArea colony = g_Game->Self()->GetColonyMapArea();
    Vector2 colonyTile = colony.Pos();

    // Get the unit type object
    UnitType type;
    TID unitTypeId;
    string typeName;
    unitTypeId = g_Database.EntityMapping.GetBySecond(p_buildingType);
    typeName = g_Database.EntityIdentMapping.GetByFirst(unitTypeId);
    type = BWAPI::UnitType::getType(typeName);

    if (type.isRefinery())
    {
        int bestDistance = INT_MAX;
        Unitset geysers = Broodwar->getGeysers();
        Unit currentGeyser;

        for (size_t i = 0; i < geysers.size(); ++i)
        {
            currentGeyser = geysers[i];
            Vector2 currentPosition = Vector2(
                UnitPositionFromTilePosition(currentGeyser->getTilePosition().x),
                UnitPositionFromTilePosition(currentGeyser->getTilePosition().y));
            int currentDistance = colonyTile.Distance(currentPosition);

            if (currentDistance <= bestDistance)
            {
                bestDistance = currentDistance;
                candidatePosition = MapArea(
                    currentPosition,
                    type.tileWidth(),
                    type.tileHeight());
            }
        }
        _ASSERTE(CanBuildHere(candidatePosition.Pos(), p_buildingType));
    }

    return candidatePosition;
}
//----------------------------------------------------------------------------------------------
bool StarCraftMap::CanBuildHere(Vector2 p_worldPosition, EntityClassType p_buildingType) const
{
    UnitType type;
    TID unitTypeId;
    string typeName;

    unitTypeId = g_Database.EntityMapping.GetBySecond(p_buildingType);
    typeName = g_Database.EntityIdentMapping.GetByFirst(unitTypeId);
    type = BWAPI::UnitType::getType(typeName);

    if (type.isAddon())
    {
        type = type.whatBuilds().first;
    }
    
    return Broodwar->canBuildHere(TilePosition(TilePositionFromUnitPosition(p_worldPosition.X), TilePositionFromUnitPosition(p_worldPosition.Y)), type);
}