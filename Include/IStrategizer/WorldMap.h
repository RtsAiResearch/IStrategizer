#ifndef WORLDMAP_H
#define WORLDMAP_H

#ifndef STATICCOMPONENT_H
#include "WorldComponent.h"
#endif
#ifndef VECTOR2_H
#include "Vector2.h"
#endif

namespace MetaData
{
    enum TileFlagType;
}

namespace IStrategizer
{
	class CellFeature;
    using namespace MetaData;

	class WorldMap
    {
	private:
		void			Initialize();

	protected:
		CellFeature**	m_cellFeatureMatrix;
		unsigned		m_cellFeatureWidth;
		unsigned		m_cellFeatureHeight;
		unsigned		m_cellFeatureMatrixWidth;
		unsigned		m_cellFeatureMatrixHeight;
		bool			m_initialized;

    public:
		WorldMap(unsigned p_cellFeatureWidth,unsigned p_cellFeatureHeight);
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