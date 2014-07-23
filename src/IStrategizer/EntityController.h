#ifndef ENTITYCONTROLLER_H
#define ENTITYCONTROLLER_H

#include "EngineData.h"
#include "EngineObject.h"
#include "Vector2.h"

namespace IStrategizer
{
    class RtsGame;
     
    class EntityController : public EngineObject
    {
    public:
        EntityController() :
        m_entityId(INVALID_TID)
        {}

        void Update(_In_ RtsGame& game) {}
        void ControlEntity(_In_ TID entityId);
        void ReleaseEntity();
        void TargetPosition(_In_ Vector2 pos) {}
        TID EntityId() const { return m_entityId; }

    private:
        TID m_entityId;
    };
}

#endif // !ENTITYCONTROLLER_H
