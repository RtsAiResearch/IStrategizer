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
        MapArea AdaptPositionForBuilding(EntityClassType p_buildingType);
        TID AdaptWorkerForBuild();
        TID AdaptBuildingForTraining(EntityClassType p_traineeType);
        TID AdaptBuildingForResearch(ResearchType p_researchType);
        TID AdaptAttacker(EntityClassType p_attackerType);
        TID AdaptEntityToMove(EntityClassType p_EntityType);
        TID AdaptTargetEntity(EntityClassType p_targetType, const PlanStepParameters& p_parameters);
        Vector2 AdaptPosition(const PlanStepParameters& p_parameters);

    private:
        Vector2 GetBotColonyCenter();

        static bool IsValidWorkerState(ObjectStateType p_workerState);
        static unsigned GetWorkerStateIndex(ObjectStateType p_workerState);
        static bool WorkerStatesComparer(pair<TID, ObjectStateType> &p_leftWorker, pair<TID, ObjectStateType> &p_rightWorker);

        static bool IsValidAttackerState(ObjectStateType p_attackerState);
        static unsigned GetAttackerStateIndex(ObjectStateType p_attackerState);
        static bool AttackerStatesComparer(pair<TID, ObjectStateType> &p_leftAttacker, pair<TID, ObjectStateType> &p_rightAttacker);

        static bool IsValidEntityToMoveState(ObjectStateType p_entityState);
        static unsigned GetEntityToMoveStateIndex(ObjectStateType p_entityState);
        static bool EntityToMoveStatesComparer(pair<TID, ObjectStateType> &p_leftAttacker, pair<TID, ObjectStateType> &p_rightAttacker);

        static bool BuildPositionSearchPredicate(unsigned p_cellX, unsigned p_cellY, const TCell* p_pCell, void *p_pParam);

        const static unsigned WorkerStatesSize;
        const static ObjectStateType WorkerStatesRank[];
        const static unsigned AttackerStatesSize;
        const static ObjectStateType AttackerStatesRank[];
        const static unsigned EntityToMoveStatesSize;
        const static ObjectStateType EntityToMoveStatesRank[];
        const static int DefaultBuildingSpacing;

        Vector2 m_botColonyCenter;
        int m_buildingSpacing;
    };
}

#endif // ADAPTEREX_H
