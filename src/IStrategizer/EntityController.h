#ifndef ENTITYCONTROLLER_H
#define ENTITYCONTROLLER_H

#include "EngineDefs.h"
#include "EngineData.h"
#include "EngineObject.h"
#include "Vector2.h"
#include "StackFSM.h"

namespace IStrategizer
{
    class RtsGame;
    class ArmyController;
     
    class EntityController : public EngineObject
    {
    public:
        static const int PositionArriveRadius = 64;

        EntityController(ArmyController* pController) :
            m_entityId(INVALID_TID),
            m_targetEntityId(INVALID_TID),
            m_singleTargetPos(Vector2::Inf()),
            m_pController(pController)
        {}

        void Update();
        void SetController(ArmyController* pController) { m_pController = pController; }
        void ControlEntity(_In_ TID entityId);
        void SetLogic(_In_ StackFSMPtr pLogic) { m_pLogic = pLogic; }
        void ReleaseEntity();
        bool IsControllingEntity() const{ return m_entityId != INVALID_TID; }
        bool IsLogicGoalAchieved() const { return m_pLogic->IsInFinalState(); }
        void ResetLogic() { m_pLogic->Reset(); }

        // Expensive Helpers are candidate for caching somewhere
        TID GetClosestEnemyEntityInSight(); // Expensive call

        // Controller Parameters
        GameEntity* Entity();
        TID EntityId() const { return m_entityId; }
        Vector2 TargetPosition() const { return m_singleTargetPos; }
        void TargetPosition(_In_ Vector2 pos) { m_singleTargetPos = pos; }
        const std::vector<Vector2>& MultiTargetPosition() { return m_multiTargetPos; }
        void MultiTargetPosition(_In_ const std::vector<Vector2>& multiPos) { m_multiTargetPos = multiPos; }
        void TargetEntity(_In_ TID entityId) { m_targetEntityId = entityId; }
        TID TargetEntity() const { return m_targetEntityId; }

        // Controller Conditions
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
        StackFSMPtr m_pLogic;
        ArmyController* m_pController;
    };

    typedef std::shared_ptr<EntityController> EntityControllerPtr;
    typedef std::shared_ptr<const EntityController> ConstEntityControllerPtr;
}

#endif // !ENTITYCONTROLLER_H
