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
        virtual TID AdaptBuildingForTraining(EntityClassType p_traineeType) = 0;
        virtual TID AdaptBuildingForResearch(ResearchType p_researchType) = 0;
        virtual TID AdaptTargetEntity(EntityClassType p_targetType, const PlanStepParameters& p_parameters) = 0;
        virtual TID AdaptResourceForGathering(ResourceType p_resourceType, const PlanStepParameters& p_parameters, const TID& p_gathererID) = 0;
        virtual TID GetEntityObjectId(EntityClassType p_entityType,const std::vector<ObjectStateType>& p_rankedStates) = 0;
        virtual TID GetEntityObjectId(EntityClassType p_entityType) = 0;
        virtual Vector2 AdaptPosition(const PlanStepParameters& p_parameters) = 0;
        virtual Vector2 AdaptEnemyBorder() = 0;
    };
}

#endif // ABSTRACTADAPTER_H
