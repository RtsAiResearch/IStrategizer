#ifndef ENTITYCONTROLLER_H
#define ENTITYCONTROLLER_H

#include "EngineDefs.h"
#include "EngineData.h"
#include "EngineObject.h"
#include "Vector2.h"
#include "StackFSM.h"
#include <unordered_map>

namespace IStrategizer
{
    class RtsGame;
    class ArmyController;
     
    class EntityController : public EngineObject
    {
    public:
        static const int PositionArriveRadius = 64;
        static const int MeleeAttackerSafetyRadius = 96;
        static const float CriticalHpPercent;
        static const float DamagedHealthPercent;
        static const float HealthyHpPercent;

        EntityController(ArmyController* pController);
        void Update();
        void ControlEntity(_In_ TID entityId);

        void PushLogic(_In_ StackFSMPtr pLogic, _In_ const EngineObject* pLogicOwner) 
        {
            LogInfo("Push %s, logic memory size = %d", pLogic->ToString().c_str(), m_pLogicMemory.size() + 1);
            m_pLogicMemory.push(make_pair(pLogic, pLogicOwner)); 
        }

        void PopLogic(_In_ const EngineObject* pLogicOwner)
        {
            _ASSERTE(m_pLogicMemory.size() > 0);
            // Who pushes a logic should be the one who pop it, this is to keep object in good state
            _ASSERTE(m_pLogicMemory.top().second == pLogicOwner);
            LogInfo("Pop %s, logic memory size = %d", m_pLogicMemory.top().first->ToString().c_str(), m_pLogicMemory.size() - 1);
            m_pLogicMemory.pop();
            LogInfo("Current logic is %s", (m_pLogicMemory.empty() ? "not-assigned" : m_pLogicMemory.top().first->ToString().c_str()));
        }
        size_t LogicMemorySize() const { return m_pLogicMemory.size(); }
        std::pair<StackFSMPtr, const EngineObject*> CurrentLogic() const { return m_pLogicMemory.top(); }
        void PushIdleLogic();
        void ReleaseEntity();
        bool IsControllingEntity() const{ return m_entityId != INVALID_TID; }
        bool IsLogicGoalAchieved() const { return IsControllingEntity() && m_pLogicMemory.top().first->IsInFinalState(); }
        void OnEntityFleeing();
        void HardResetLogic();
        void SoftResetLogic() { m_pLogicMemory.top().first->Reset(); }
        std::string ToString(bool minimal = false) const;
        GameEntity* Entity() { return m_pEntity; }
        const GameEntity* Entity() const { return m_pEntity; }
        TID EntityId() const { return m_entityId; }
        EntityClassType TypeId() const { return m_typeId; }
        void DebugDraw() { if (!m_pLogicMemory.empty()) m_pLogicMemory.top().first->DebugDraw(); }

        // Expensive Helpers are candidate for caching somewhere
        TID Attacker() const;
        TID GetClosestEnemyEntityInSight();
        void CalcCloseMeleeAttacker();
        TID ChooseRepairTarget();
        TID CloseMeleeAttacker() const { return m_closeMeleeAttackerId; }

        // Controller Input Parameters
        Vector2 TargetPosition() const;
        void TargetPosition(_In_ Vector2 pos) { m_singleTargetPos = pos; }
        void TargetEntity(_In_ TID entityId) { m_targetEntityId = entityId; }
        const std::vector<Vector2>& MultiTargetPosition() { return m_multiTargetPos; }
        void MultiTargetPosition(_In_ const std::vector<Vector2>& multiPos) { m_multiTargetPos = multiPos; }
        TID TargetEntity() const;

        // Controller Conditions
        bool IsOnCriticalHP() const;
        bool IsOnHealthyHP() const;
        bool IsBeingHit() const;
        bool ArrivedAtTarget(_In_ Vector2 pos) const;
        bool ThreatAtTarget(_In_ Vector2 pos) const;
        bool IsTargetInSight(_In_ Vector2 pos) const;
        bool IsTargetInSight(_In_ TID entityId) const;
        bool IsTargetInRange(_In_ TID entityId) const;
        bool EntityExists() const;
        bool IsAnyEnemyTargetInSight() const; // Expensive call
        bool IsAnyEnemyTargetInRange() const; // Expensive call
        bool IsCloseToMeleeAttacker() const { return m_closeMeleeAttackerId != INVALID_TID; }
        bool CanRepairNearbyEntity() const;
        static bool IsHpAboveThreshold(_In_ const GameEntity* pEntity, _In_ float hpThresholdPrcnt);
        static bool EntityExists(_In_ TID entityId);
        static bool IsDamaged(_In_ const GameEntity* pEntity);

    private:
        DISALLOW_COPY_AND_ASSIGN(EntityController);

        TID m_entityId;
        EntityClassType m_typeId;
        GameEntity* m_pEntity;
        TID m_targetEntityId;
        Vector2 m_singleTargetPos;
        std::vector<Vector2> m_multiTargetPos;
        ArmyController* m_pController;
        std::stack<std::pair<StackFSMPtr, const EngineObject*>> m_pLogicMemory;
        TID m_closeMeleeAttackerId;
    };

    typedef std::shared_ptr<EntityController> EntityControllerPtr;
    typedef std::shared_ptr<const EntityController> ConstEntityControllerPtr;
    typedef std::unordered_map<TID, EntityControllerPtr> EntityControllersMap;

}

#endif // !ENTITYCONTROLLER_H
