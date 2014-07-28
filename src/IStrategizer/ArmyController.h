#ifndef BATTLE_H
#define BATTLE_H

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

    class ArmyController : public EngineObject
    {
    public:
        ArmyController(StrategySelectorPtr pConsultant);

        static const int FocusAreaRadius = 192;
        static const int SightAreaRadius = 512;

        void Update();
        void AttackArea(_In_ Vector2 pos) {}
        void DefendArea(_In_ Vector2 pos);
        void NotifyMessegeSent(_In_ Message* pMsg);
        bool IsControllingArmy() const { return !m_entities.empty(); }
        bool HasType(_In_ EntityClassType type);
        void ReleaseEntity(_In_ TID entityId);
        void ControlEntity(_In_ TID entityId);
        void ControlArmy();
        void ReleaseArmy();
        Vector2 Center() const { return m_center; }
        Circle2 FocusArea() { return Circle2(Center(), FocusAreaRadius); }
        Circle2 SightArea() { return Circle2(Center(), SightAreaRadius); }
        // Expensive Helpers are candidate for caching somewhere
        TID GetClosestEnemyEntityInSight() const { _ASSERTE(!m_closestEnemyInSight.empty()); return m_closestEnemyInSight.begin()->second; }
        const std::multimap<int, TID>& GetClosestEnemiesInSight() const { return m_closestEnemyInSight; }
        void OnEntityDestroyed(_In_ TID entityId);
        void OnEntityFleeing(_In_ TID entityId);
        bool IsControllingEntity(_In_ TID entityId) const { return m_entities.count(entityId) > 0; }
        // Controller Input Parameters
        Vector2 TargetPosition() const { return m_singleTargetPos; }
        void TargetPosition(_In_ Vector2 pos) { m_singleTargetPos = pos; }
        TID TargetEntity() const { return m_currentTarget; }
        bool CanControl(_In_ const GameEntity* pEntity);

        // Controller Conditions
        bool IsAnyEnemyTargetInSight() const { return !m_closestEnemyInSight.empty(); }
        //bool IsInOrder() const { return m_isInOrder; }
        static bool IsInOrder(const EntityControllersMap& entities, _In_ Vector2 pos);

        const EntityControllersMap& HealthyEntities() const { return m_entities; }

    private:
        ArmyGroup Classify(const GameType* pType);
        void CalcCetner();
        void CalcClosestEnemyInSight();
        void CalcIsInOrder();

        std::unordered_map<TID, EntityControllerPtr> m_entities;
        std::set<TID> m_currFramefleeingEntities;

        StackFSMPtr m_pLogic;
        TID m_currentTarget;
        StrategySelectorPtr m_pConsultant;
        Vector2 m_singleTargetPos;
        Vector2 m_center;
        std::multimap<int, TID> m_closestEnemyInSight;
        bool m_isInOrder;
    };
}

#endif // BATTLE_H