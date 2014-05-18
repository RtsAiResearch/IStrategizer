///> [Serializable]
#ifndef WORLDMAP_H
#define WORLDMAP_H

#include <vector>
#ifndef VECTOR2_H
#include "Vector2.h"
#endif

namespace IStrategizer
{
    enum TileFlagType;
    enum EntityClassType;
    class CellFeature;
    class MapArea;
    class RtsGame;

    ///> class=WorldMap
    class WorldMap
    {
    public:
        WorldMap(unsigned p_cellWidth, unsigned p_cellHeight, unsigned p_worldWidth, unsigned p_worldHeight);
        ~WorldMap();

        void Init();
        virtual Vector2 Size() const { return Vector2(m_worldWidth, m_worldHeight); }
        virtual bool CanBuildHere(Vector2 p_position, EntityClassType p_buildingType) const = 0;
        virtual MapArea GetSpecialBuildingPosition(EntityClassType p_buildingType) const = 0;
        void Update();
        Vector2 CellMatrixSize() const;
        Vector2 CellSize() const;
        Vector2 GetNearestCell(CellFeature* p_cell) const;
        CellFeature* GetCellFeatureFromWorldPosition(Vector2 p_position) const;
        std::vector<Vector2> GetNearestEnemyBorders(int p_numberOfBorders);
        void SetOffline(RtsGame* pBelongingGame);

    protected:
        CellFeature** m_cellFeatureMatrix;
        unsigned m_cellSide;
        unsigned m_numCells;
        unsigned m_worldWidth;
        unsigned m_worldHeight;
        unsigned m_gridWidth;
        unsigned m_gridHeight;
        bool m_initialized;
        RtsGame* m_pGame;
        bool m_isOnline;

        Vector2 FromGridToWorld(const Vector2 &p_gridPosition) const;
        Vector2 FromWorldToGrid(const Vector2 &p_worldPosition) const;
    };
}

#endif // WORLDMAP_H
