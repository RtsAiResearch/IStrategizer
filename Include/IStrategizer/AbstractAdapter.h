#ifndef ABSTRACTADAPTER_H
#define ABSTRACTADAPTER_H

#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif
#ifndef MAPAREA_H
#include "MapArea.h"
#endif

namespace IStrategizer
{
    class Vector2;
    class CellFeature;

    class AbstractAdapter
    {
    public:
        virtual MapArea AdaptPositionForBuilding(EntityClassType p_buildingType) = 0;
        virtual TID AdaptWorkerForBuild() = 0;
        virtual TID AdaptBuildingForTraining(EntityClassType p_traineeType) = 0;
        virtual TID AdaptBuildingForResearch(ResearchType p_researchType) = 0;
        virtual TID AdaptAttacker(EntityClassType p_attackerType) = 0;
        virtual TID AdaptEntityToMove(EntityClassType p_EntityType) = 0;
        virtual TID AdaptTargetEntity(EntityClassType p_targetType, const PlanStepParameters& p_parameters) = 0;
        virtual Vector2 AdaptPosition(const PlanStepParameters& p_parameters) = 0;
    };
}

#endif // ABSTRACTADAPTER_H
