#ifndef ABSTRACTADAPTER_H
#define ABSTRACTADAPTER_H

#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif

class Vector2;

namespace IStrategizer
{
	class CellFeature;
}

namespace OLCBP
{
	using namespace IStrategizer;
	using namespace MetaData;

	class AbstractAdapter
	{
	public:
		virtual Vector2 AdaptPositionForBuilding(EntityClassType p_buildingType) = 0;
		virtual	TID AdaptWorkerForBuild() = 0;
		virtual TID AdaptBuildingForTraining(EntityClassType p_traineeType) = 0;
		virtual TID AdaptBuildingForResearch(ResearchType p_researchType) = 0;
	};
}

#endif	// ABSTRACTADAPTER_H