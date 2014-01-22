#ifndef ABSTRACTADAPTER_H
#define ABSTRACTADAPTER_H

#include "EngineData.h"
#include "MapArea.h"
#include "RtsGame.h"

namespace IStrategizer
{
    class Vector2;
    class CellFeature;

    class AbstractAdapter
    {
    public:
        virtual MapArea AdaptPositionForBuilding(RtsGame& p_RtsGame, EntityClassType p_buildingType) = 0;
        virtual TID AdaptBuildingForTraining(RtsGame& p_RtsGame, EntityClassType p_traineeType) = 0;
        virtual TID AdaptBuildingForResearch(RtsGame& p_RtsGame, ResearchType p_researchType) = 0;
        virtual TID AdaptTargetEntity(RtsGame& p_RtsGame, EntityClassType p_targetType, const PlanStepParameters& p_parameters) = 0;
        virtual TID GetEntityObjectId(RtsGame& p_RtsGame, EntityClassType p_entityType,const vector<ObjectStateType>& p_rankedStates) = 0;
        virtual TID GetEntityObjectId(RtsGame& p_RtsGame, EntityClassType p_entityType) = 0;
        virtual Vector2 AdaptPosition(RtsGame& p_RtsGame, const PlanStepParameters& p_parameters) = 0;
    };
}

#endif // ABSTRACTADAPTER_H
