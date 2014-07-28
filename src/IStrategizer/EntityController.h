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

        EntityController(ArmyController* pController);
        void Update();
        void SetController(ArmyController* pController) { m_pController = pController; }
        void ControlEntity(_In_ TID entityId);
        void PushLogic(_In_ StackFSMPtr pLogic) { m_pLogicMemory.push(pLogic); }
        void PopLogic() { m_pLogicMemory.pop(); }
        void PushIdleLogic();
        void ReleaseEntity();
        bool IsControllingEntity() const{ return m_entityId != INVALID_TID; }
        bool IsLogicGoalAchieved() const { return m_pLogicMemory.top()->IsInFinalState(); }
        void OnEntityFleeing();
        void HardResetLogic();
        void SoftResetLogic() { m_pLogicMemory.top()->Reset(); }

        GameEntity* Entity();
        const GameEntity* Entity() const;
        TID EntityId() const { return m_entityId; }
        // Expensive Helpers are candidate for caching somewhere
        TID GetClosestEnemyEntityInSight(); // Expensive call

        // Controller Input Parameters
        Vector2 TargetPosition() const;
        void TargetPosition(_In_ Vector2 pos) { m_singleTargetPos = pos; }
        const std::vector<Vector2>& MultiTargetPosition() { return m_multiTargetPos; }
        void MultiTargetPosition(_In_ const std::vector<Vector2>& multiPos) { m_multiTargetPos = multiPos; }
        void TargetEntity(_In_ TID entityId) { m_targetEntityId = entityId; }
        TID TargetEntity() const { return m_targetEntityId; }

        // Controller Conditions
        static bool IsOnCriticalHP(_In_ const GameEntity* pEntity);
        bool IsOnCriticalHP();
        bool IsBeingHit();
        bool ArrivedAtTarget(_In_ Vector2 pos);
        bool ThreatAtTarget(_In_ Vector2 pos);
        bool IsTargetInSight(_In_ Vector2 pos);
        bool IsTargetInSight(_In_ TID entityId);
        bool EntityExists() const;
        bool EntityExists(_In_ TID entityId) const;
        bool IsAnyEnemyTargetInSight(); // Expensive call

    private:
        DISALLOW_COPY_AND_ASSIGN(EntityController);

        TID m_entityId;
        TID m_targetEntityId;
        Vector2 m_singleTargetPos;
        std::vector<Vector2> m_multiTargetPos;
        ArmyController* m_pController;
        std::stack<StackFSMPtr> m_pLogicMemory;
    };

    typedef std::shared_ptr<EntityController> EntityControllerPtr;
    typedef std::shared_ptr<const EntityController> ConstEntityControllerPtr;
    typedef std::unordered_map<TID, EntityControllerPtr> EntityControllersMap;

}

#endif // !ENTITYCONTROLLER_H
