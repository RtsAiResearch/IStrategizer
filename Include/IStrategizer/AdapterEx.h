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
    };

    class AdapterEx : public AbstractAdapter
    {
    public:
        AdapterEx();
        MapArea AdaptPositionForBuilding(RtsGame& p_RtsGame, EntityClassType p_buildingType);
        TID AdaptBuildingForTraining(RtsGame& p_RtsGame, EntityClassType p_traineeType);
        TID AdaptBuildingForResearch(RtsGame& p_RtsGame, ResearchType p_researchType);
        TID AdaptTargetEntity(RtsGame& p_RtsGame, EntityClassType p_targetType, const PlanStepParameters& p_parameters);
        TID GetEntityObjectId(RtsGame& p_RtsGame, EntityClassType p_entityType,const vector<ObjectStateType>& p_rankedStates);
        TID GetEntityObjectId(RtsGame& p_RtsGame, EntityClassType p_entityType);
        Vector2 AdaptPosition(RtsGame& p_RtsGame, const PlanStepParameters& p_parameters);

        static vector<ObjectStateType> WorkerStatesRankVector;
        static vector<ObjectStateType> AttackerStatesRankVector;
        static vector<ObjectStateType> EntityToMoveStatesRankVector;

    private:
        Vector2 GetBotColonyCenter(RtsGame& p_RtsGame);
        static void InitializePredefinedRankedStates();
        static bool EntityToMoveStatesComparer(pair<TID, ObjectStateType> &p_leftAttacker, pair<TID, ObjectStateType> &p_rightAttacker);
        static bool IsValidEntityState(ObjectStateType p_entityState, const vector<ObjectStateType>& p_rankedStates);
        static int GetEntityStateIndex(ObjectStateType p_entityState, const vector<ObjectStateType>& p_rankedStates);
        static bool BuildPositionSearchPredicate(unsigned p_cellX, unsigned p_cellY, const TCell* p_Cell, void *p_Param);

        const static int DefaultBuildingSpacing;
        static bool IsRankedStatesInitialized;
        Vector2 m_botColonyCenter;
        int m_buildingSpacing;

    };
}

#endif // ADAPTEREX_H
