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

	class WorldMap
    {
	private:
		void			Initialize();
		Vector2			FromGridToWorld(const Vector2 &p_gridPosition) const;

	protected:
		CellFeature**	m_cellFeatureMatrix;
		int				m_cellSide;
		int				m_numCells;
		int				m_worldWidth;
		int				m_worldHeight;
		int				m_gridWidth;
		int				m_gridHeight;
		bool			m_initialized;

    public:
		WorldMap(int p_cellWidth, int p_cellHeight, int p_worldWidth, int p_worldHeight);
		~WorldMap();

        virtual Vector2         Size() const = 0;
		Vector2					CellMatrixSize() const;
		Vector2					CellSize() const;
		void					UpdateAux();
		CellFeature*			GetCellFeature(Vector2 p_position) const;
		Vector2					GetNearestCell(CellFeature* p_cell, double p_threshold) const;
		bool					IsBuildable(Vector2 p_position, Vector2 p_dimension) const ;

    };
}

#endif // WORLDMAP_H