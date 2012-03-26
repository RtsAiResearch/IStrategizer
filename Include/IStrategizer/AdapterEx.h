#ifndef ADAPTEREX_H
#define ADAPTEREX_H

#ifndef ABSTRACTADAPTER_H
#include "AbstractAdapter.h"
#endif
#ifndef VECTOR2_H
#include "Vector2.h"
#endif
#ifndef INFLUENCEMAP_H
#include "InfluenceMap.h"
#endif

namespace IStrategizer
{
	class CellFeature;
	class WorldMap;
}

namespace OLCBP
{
	using namespace IStrategizer;

	struct SpiralSearchData
	{
		int BuildingWidth;
		int BuildingHeight;
		Vector2 CandidateBuildPos;
	};

	class AdapterEx : public AbstractAdapter
	{
	public:
		AdapterEx();
		Vector2 AdaptPositionForBuilding(EntityClassType p_buildingType);
		TID AdaptWorkerForBuild();
		TID AdaptBuildingForTraining(EntityClassType p_traineeType);
		TID AdaptBuildingForResearch(ResearchType p_researchType);

	private:
		Vector2 GetBotColonyCenter();
		static bool IsValidWorkerState(ObjectStateType p_workerState);
		static unsigned GetWorkerStateIndex(ObjectStateType p_workerState);
		static bool WorkerStatesComparer(pair<TID, ObjectStateType> &p_leftWorker, pair<TID, ObjectStateType> &p_rightWorker);
		static bool BuildPositionSearchPredicate(unsigned p_cellX, unsigned p_cellY, const TCell* p_pCell, void *p_pParam);

		const static unsigned			WorkerStatesSize;
		const static ObjectStateType	WorkerStatesRank[];
		const static int				DefaultBuildingSpacing;

		Vector2 m_botColonyCenter;
		int		m_buildingSpacing;
	};
}

#endif	// ADAPTEREX_H