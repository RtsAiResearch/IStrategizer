#ifndef OCCUPANCEDATAIM_H
#define OCCUPANCEDATAIM_H

#include "InfluenceMap.h"
#include "RtsGame.h"

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
        void Update(RtsGame& p_RtsGame, const WorldClock& p_clock);
        void UnregisterGameObj(TID p_objectId);
        bool IsAreaOccupied(const Vector2& p_areaPos, int p_areaWidth, int p_areaHeight);
        bool ReserveArea(const Vector2& p_areaPos, int p_areaWidth, int p_areaHeight);
        bool FreeArea(const Vector2& p_areaPos, int p_areaWidth, int p_areaHeight);

    private:
        static bool OccupancePredicate(unsigned p_cellX, unsigned p_cellY, TCell* p_Cell, void *p_Param);
        static bool ReservePredicate(unsigned p_worldX, unsigned p_worldY, TCell* p_Cell, void *p_Param);
        static bool FreePredicate(unsigned p_worldX, unsigned p_worldY, TCell* p_Cell, void *p_Param);
    };
}

#endif // OCCUPANCEDATAIM_H
