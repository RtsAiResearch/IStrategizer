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
        const int PositionArriveRadius = 64;

        EntityController() :
        m_entityId(INVALID_TID),
        m_targetPos(Vector2::Inf())
        {}

        void Update();
        void ControlEntity(_In_ TID entityId, _In_ StackFSMPtr logic);
        void ReleaseEntity();
        void TargetPosition(_In_ Vector2 pos) { m_targetPos = pos; }
        Vector2 TargetPosition() const { return m_targetPos; }
        TID EntityId() const { return m_entityId; }
        GameEntity* Entity();
        bool IsControllingEntity() const{ return m_entityId != INVALID_TID; }
        bool EntityExist() const;

        // Controller Conditions
        bool IsOnCriticalHP();
        bool IsBeingHit();
        bool ArrivedAtTarget();

    private:
        DISALLOW_COPY_AND_ASSIGN(EntityController);

        TID m_entityId;
        Vector2 m_targetPos;
        StackFSMPtr m_logic;
    };
}

#endif // !ENTITYCONTROLLER_H
