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

#define INVALID_INDEX (-1)

namespace IStrategizer
{
    class CellFeature;
    class WorldMap;

    struct SpiralSearchData
    {
        int BuildingWidth;
        int BuildingHeight;
        Vector2 CandidateBuildPos;
        EntityClassType BuildingType;
    };

    class AdapterEx : public AbstractAdapter
    {
    public:
        AdapterEx();
        MapArea AdaptPositionForBuilding(EntityClassType p_buildingType);
        TID AdaptBuildingForTraining(EntityClassType p_traineeType);
        TID AdaptBuildingForResearch(ResearchType p_researchType);
        TID AdaptTargetEntity(EntityClassType p_targetType, const PlanStepParameters& p_parameters);
        TID GetEntityObjectId(EntityClassType p_entityType,const std::vector<ObjectStateType>& p_rankedStates);
        TID GetEntityObjectId(EntityClassType p_entityType);
        TID AdaptResourceForGathering(ResourceType p_resourceType, const PlanStepParameters& p_parameters, const TID& p_gathererID);
        Vector2 AdaptPosition(const PlanStepParameters& p_parameters);
        Vector2 AdaptEnemyBorder();
        static std::vector<ObjectStateType> WorkerStatesRankVector;
        static std::vector<ObjectStateType> AttackerStatesRankVector;
        static std::vector<ObjectStateType> EntityToMoveStatesRankVector;

    private:
        static void InitializePredefinedRankedStates();
        static bool EntityToMoveStatesComparer(std::pair<TID, ObjectStateType> &p_leftAttacker, std::pair<TID, ObjectStateType> &p_rightAttacker);
        static bool IsValidEntityState(ObjectStateType p_entityState, const std::vector<ObjectStateType>& p_rankedStates);
        static int GetEntityStateIndex(ObjectStateType p_entityState, const std::vector<ObjectStateType>& p_rankedStates);
        static bool BuildPositionSearchPredicate(unsigned p_cellX, unsigned p_cellY, const TCell* p_pCell, void *p_pParam);
        TID GetSourceEntity(EntityClassType p_traineeType, ActionType actionType) const;
        MapArea AdaptPositionForSpecialBuilding(EntityClassType p_buildingType);
        const static int DefaultBuildingSpacing;
        static bool IsRankedStatesInitialized;
        int m_buildingSpacing;

    };
}

#endif // ADAPTEREX_H
