#ifndef OCCUPANCEDATAIM_H
#define OCCUPANCEDATAIM_H

#include "InfluenceMap.h"

namespace IStrategizer
{
    class OccupanceDataIM : public InfluenceMap
    {
    public:
        enum CellData : TInfluence
        {
            CELL_Free = 0x0,
            CELL_Reserved = 0x1,
        };

        OccupanceDataIM(IMType p_typeId) : InfluenceMap(p_typeId) {}
        void Update();
        void UnregisterGameObj(TID p_objectId);
        bool IsAreaOccupied(const Vector2& p_areaPos, int p_areaWidth, int p_areaHeight);
        bool ReserveArea(const Vector2& p_areaPos, int p_areaWidth, int p_areaHeight);
        bool FreeArea(const Vector2& p_areaPos, int p_areaWidth, int p_areaHeight);
        bool CanBuildHere(Vector2 p_worldPos, int p_buildingWidth, int p_buildingHeight, EntityClassType p_buildingType);
        void GetBuildingsInArea(Circle2 area);
        void RegisterGameObj(TID objId, PlayerType ownerId);
        bool CanBuildExpansion(GameEntity* pBuilding);

    private:
        static bool OccupancePredicate(unsigned p_cellX, unsigned p_cellY, TCell* p_pCell, void *p_pParam);
        static bool BuildingExpansionOccupancePredicate(unsigned p_cellX, unsigned p_cellY, TCell* p_pCell, void *p_pParam);
        static bool ReservePredicate(unsigned p_worldX, unsigned p_worldY, TCell* p_pCell, void *p_pParam);
        static bool FreePredicate(unsigned p_worldX, unsigned p_worldY, TCell* p_pCell, void *p_pParam);
    };
}

#endif // OCCUPANCEDATAIM_H
