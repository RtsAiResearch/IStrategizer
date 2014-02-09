#ifndef CELLFEATURE_H
#define CELLFEATURE_H

#include "ForceDescription.h"
#include "BuildingDescription.h"
#include "ResourceDescription.h"
#include "MetaData.h"
#include "EngineData.h"
#include "Vector2.h"
#include  <map>
#include "InfluenceMap.h"

namespace IStrategizer
{
    class GameEntity;

    class CellFeature
    {
    public:
        ResourceDescription m_resourceDescription;
        ForceDescription m_alliedForceDescription;
        ForceDescription m_enemyForceDescription;
        BuildingDescription m_alliedBuildingDescription;
        BuildingDescription m_enemyBuildingDescription;
        int m_distanceFromEnemyBase;
        int m_distanceFromBase;
        TInfluence m_influnce;
        CellFeature() { Clear(); }
        CellFeature(const PlanStepParameters& p_parameters);
        void AddEntity(GameEntity *p_entity,bool p_isAllied);
        void RemoveEntity(GameEntity *p_entity, bool p_isAllied);
        void Clear();
        void To(PlanStepParameters& p_parameters) const;
        void CalculateDistanceToBases(Vector2 cellWorldPosition);
        float GetDistance(CellFeature *p_other);
        float GetInflunceSimilarity (TInfluence& p_otherInflunce);

        static const CellFeature& Null() { static CellFeature nullCellFeature; return nullCellFeature; }

    private:
        void CalculateDistanceToBasesAux(Vector2 cellWorldPosition, vector<TID> bases, int& distance);
        float GetBaseDistanceSimilarity(int firstBase, int secondBase) const;
    };
}

#endif // CELLFEATURE_H