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
     
    class EntityController : public EngineObject
    {
    public:
        static const int PositionArriveRadius = 64;

        EntityController() :
            m_entityId(INVALID_TID),
            m_singleTargetPos(Vector2::Inf())
        {}

        void Update();
        void ControlEntity(_In_ TID entityId, _In_ StackFSMPtr logic);
        void ReleaseEntity();
        bool IsControllingEntity() const{ return m_entityId != INVALID_TID; }
        bool EntityExist() const;
        bool IsLogicGoalAchieved() const { return m_pLogic->IsInFinalState(); }
        void ResetLogic() { m_pLogic->Reset(); }

        // Controller Parameters
        GameEntity* Entity();
        TID EntityId() const { return m_entityId; }
        Vector2 TargetPosition() const { return m_singleTargetPos; }
        void TargetPosition(_In_ Vector2 pos) { m_singleTargetPos = pos; }
        void MultiTargetPosition(_In_ const std::vector<Vector2>& multiPos) { m_multiTargetPos = multiPos; }
        const std::vector<Vector2>& MultiTargetPosition() { return m_multiTargetPos; }

        // Controller Conditions
        bool IsOnCriticalHP();
        bool IsBeingHit();
        bool ArrivedAtTarget();
        bool ThreatAtTarget();
        bool IsTargetInSight();

    private:
        DISALLOW_COPY_AND_ASSIGN(EntityController);

        TID m_entityId;
        Vector2 m_singleTargetPos;
        std::vector<Vector2> m_multiTargetPos;
        StackFSMPtr m_pLogic;
    };
}

#endif // !ENTITYCONTROLLER_H
