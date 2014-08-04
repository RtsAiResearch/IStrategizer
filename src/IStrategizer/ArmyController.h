#ifndef ARMYCONTROLLER_H
#define ARMYCONTROLLER_H

#include "StackFSM.h"
#include "StrategySelector.h"
#include "EntityController.h"
#include <unordered_map>

namespace IStrategizer
{
    enum ArmyGroup
    {
        ARMGRP_MeleelAttacker,
        ARMGRP_RangedAttacker,
        ARMGRP_Supporter,
        ARMGRP_Worker
    };

    class ArmyEnemyData
    {
    public:
        ArmyEnemyData() :
            Id(INVALID_TID),
            TargetEntityId(INVALID_TID),
            DistanceToCenter(INT_MAX)
        {}

        TID Id;
        TID TargetEntityId;
        int DistanceToCenter;
    };

    class ArmyGroupFormation
    {
    public:
        class Data
        {
        public:
            Data() :
                MinSquareSide(0),
                SquareSide(0),
                CircleRadius(0)
            {}

            int MinSquareSide;
            int SquareSide;
            int CircleRadius;
        };

        ArmyGroupFormation() :
            Center(Vector2::Inf())
        {}

        void Reset()
        {
            Center = Vector2::Inf();
            Placement.clear();
        }

        Vector2 Center;
        std::map<TID, Vector2> Placement;
    };

    class ArmyController : public EngineObject
    {
    public:
        ArmyController(const char* pName, StrategySelectorPtr pConsultant);

        static const int FocusAreaRadius = 192;
        static const int SightAreaRadius = 768;
        static const int FormationSpacing = 48;

        void Update();
        void Attack(_In_ Vector2 pos);
        void Defend(_In_ Vector2 pos);
        void Stand(_In_ Vector2 pos);

        void NotifyMessegeSent(_In_ Message* pMsg);
        bool IsControllingArmy() const { return !m_entities.empty(); }
        bool HasType(_In_ EntityClassType type);
        void ReleaseEntity(_In_ TID entityId);
        bool TryControlEntity(_In_ TID entityId);
        const StrategySelector* Consultant() const { return &*m_pConsultant; }
        void ReleaseHealthyEntities();
        void TryControlArmy(_In_ bool fromCleanSlate);
        void ReleaseArmy();
        const StackFSM* Logic() const { return &*m_pLogic; }
        Vector2 Center() const { return m_center; }
        Circle2 FormationArea() { return Circle2(Center(), m_formationData.CircleRadius); }
        Circle2 SightArea() { return Circle2(Center(), m_formationData.CircleRadius + SightAreaRadius); }
        int TotalDiedEntities() const { return m_totalDiedEntities; }
        int TotalGroundAttack() const { return m_totalGroundAttack; }
        int TotalMaxHP() const { return m_totalMaxHP; }
        const ArmyGroupFormation::Data& FormationData() const { return m_formationData; }
        void CalcGroupFormation(_Inout_ ArmyGroupFormation& formation);
        std::string ToString(bool minimal = false) const { return m_pName; }
        void SetControlType(_In_ bool controlBroken, _In_ bool controlWorkers);
        const EntitySet& DamagedRepairablesNearby() const { return m_damagedRepairablesNearby; }
        TID ChooseRepairTarget(_In_ const GameEntity* pEntity);
        void DebugDraw() { if (m_pLogic) m_pLogic->DebugDraw(); }

        // Expensive Helpers are candidate for caching somewhere
        TID GetClosestEnemyEntity() const { _ASSERTE(!m_closestEnemy.empty()); return m_closestEnemy.begin()->second; }
        const std::multimap<int, TID>& ClosestEnemyEntities() const { return m_closestEnemy; }
        void OnEntityDestroyed(_In_ TID entityId);
        void OnEntityFleeing(_In_ TID entityId);
        bool IsControllingEntity(_In_ TID entityId) const { return m_entities.count(entityId) > 0; }
        const std::unordered_map<TID, ArmyEnemyData>& EnemyData() const { return m_enemyData; }

        // Controller Input Parameters
        Vector2 TargetPosition() const { return m_singleTargetPos; }
        void TargetPosition(_In_ Vector2 pos) { m_singleTargetPos = pos; }
        TID TargetEntity() const { return m_targetEntityId; }
        bool CanControl(_In_ const GameEntity* pEntity);

        // Controller Conditions
        bool IsAnyEnemyInFormationAreaSight(_In_ Vector2 areaPos) const;
        static bool IsInOrder(const EntityControllersMap& entities, _In_ Vector2 pos);
        const EntityControllersMap& Entities() const { return m_entities; }
        
        bool HasRepairers();
        bool HasDamagedEntities() const;
        void ControlRepairers();
        void ReleaseRepairers();

    private:
        DISALLOW_COPY_AND_ASSIGN(ArmyController);

        ArmyGroup Classify(const GameType* pType);
        void CalcCetner();
        void CalcEnemyData();
        void CalcIsFormationInOrder();
        void CalcBoundingCircleRadius();
        void CalcGroupFormationData();
        void CalcDamagedRepairablesNearby();

        static ArmyGroupFormation::Data CalcGroupFormationData(_In_ int groupSize);

        std::unordered_map<TID, EntityControllerPtr> m_entities;
        std::set<TID> m_currFramefleeingEntities;
        StackFSMPtr m_pLogic;
        TID m_targetEntityId;
        StrategySelectorPtr m_pConsultant;
        Vector2 m_singleTargetPos;
        Vector2 m_center;
        std::unordered_map<TID, ArmyEnemyData> m_enemyData;
        std::set<TID> m_enemiesInSight;
        std::multimap<int, TID> m_closestEnemy;
        ArmyGroupFormation::Data m_formationData;
        EntitySet m_damagedRepairablesNearby;

        static std::unordered_map<int, ArmyGroupFormation::Data> sm_cachedArmySizeToFormationDataMap;

        int m_boundingCircleRadius;
        const char* m_pName;
        
        // Control Type
        bool m_controlBroken;
        bool m_controlWorkers;

        // Statistics
        int m_totalDiedEntities;
        int m_totalGroundAttack;
        int m_totalMaxHP;
    };

    typedef std::shared_ptr<ArmyController> ArmyControllerPtr;
}

#endif // ARMYCONTROLLER_H