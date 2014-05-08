#ifndef WORLDMAP_H
#define WORLDMAP_H

#ifndef STATICCOMPONENT_H
#include "WorldComponent.h"
#endif
#ifndef VECTOR2_H
#include "Vector2.h"
#endif

namespace IStrategizer
{
    enum TileFlagType;
    class CellFeature;
    class MapArea;

    class WorldMap
    {
    private:
        void Initialize();

    protected:
        CellFeature** m_cellFeatureMatrix;
        unsigned m_cellSide;
        unsigned m_numCells;
        unsigned m_worldWidth;
        unsigned m_worldHeight;
        unsigned m_gridWidth;
        unsigned m_gridHeight;
        bool m_initialized;

        Vector2 FromGridToWorld(const Vector2 &p_gridPosition) const;
        Vector2 FromWorldToGrid(const Vector2 &p_worldPosition) const;

    public:
        WorldMap(unsigned p_cellWidth, unsigned p_cellHeight, unsigned p_worldWidth, unsigned p_worldHeight);
        ~WorldMap();

        virtual Vector2 Size() const = 0;
        virtual bool CanBuildHere(Vector2 p_position, EntityClassType p_buildingType) const = 0;
        virtual MapArea GetSpecialBuildingPosition(EntityClassType p_buildingType) const = 0;
        void UpdateAux();
        Vector2 CellMatrixSize() const;
        Vector2 CellSize() const;
        Vector2 GetNearestCell(CellFeature* p_cell) const;
        CellFeature* GetCellFeatureFromWorldPosition(Vector2 p_position) const;
        std::vector<Vector2> GetNearestEnemyBorders(int p_numberOfBorders);

    };
}

#endif // WORLDMAP_H
