#ifndef ABSTRACTADAPTER_H
#define ABSTRACTADAPTER_H

#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif
#ifndef MAPAREA_H
#include "MapArea.h"
#endif

class Vector2;

namespace IStrategizer
{
	class CellFeature;

	class AbstractAdapter
	{
	public:
		virtual MapArea AdaptPositionForBuilding(EntityClassType p_buildingType) = 0;
		virtual	TID AdaptWorkerForBuild() = 0;
		virtual TID AdaptBuildingForTraining(EntityClassType p_traineeType) = 0;
		virtual TID AdaptBuildingForResearch(ResearchType p_researchType) = 0;
	};
}

#endif	// ABSTRACTADAPTER_H