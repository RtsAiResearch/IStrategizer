#ifndef WORLDMAP_H
#define WORLDMAP_H

#ifndef STATICCOMPONENT_H
#include "WorldComponent.h"
#endif
#ifndef VECTOR2_H
#include "Vector2.h"
#endif
#include "IClonable.h"

namespace IStrategizer
{
    enum TileFlagType;
    class CellFeature;

    class WorldMap : public IClonable
    {
    private:
        void Initialize();
        Vector2 FromGridToWorld(const Vector2 &p_gridPosition) const;
        Vector2 FromWorldToGrid(const Vector2 &p_worldPosition) const;

    protected:
        WorldMap() {}

        CellFeature** m_cellFeatureMatrix;
        unsigned        m_cellSide;
        unsigned        m_numCells;
        unsigned        m_worldWidth;
        unsigned        m_worldHeight;
        unsigned        m_gridWidth;
        unsigned m_gridHeight;
        bool m_initialized;

    public:
        WorldMap(unsigned p_cellWidth, unsigned p_cellHeight, unsigned p_worldWidth, unsigned p_worldHeight);
        ~WorldMap();

        virtual Vector2 Size() const = 0;
        Vector2 CellMatrixSize() const;
        Vector2 CellSize() const;
        void UpdateAux();
        CellFeature* GetCellFeatureFromWorldPosition(Vector2 p_position) const;
        Vector2 GetNearestCell(CellFeature* p_cell) const;
        std::vector<Vector2> GetNearestEnemyBorders(int p_numberOfBorders);
        bool IsBuildable(Vector2 p_position, Vector2 p_dimension) const ;

        void Copy(IClonable* pDest);
    };
}

#endif // WORLDMAP_H
