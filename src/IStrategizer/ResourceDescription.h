///> [Serializable]
#ifndef RESOURCEDESCRIPTION_H
#define RESOURCEDESCRIPTION_H

#include "SVector.h"
#include "MetaData.h"
#include "EngineObject.h"

namespace IStrategizer
{
    class GameEntity;

    ///> class=ResourceDescription
    class ResourceDescription : public EngineObject
    {
        OBJECT_SERIALIZABLE(ResourceDescription, &m_numberOfPrimary, &m_numberOfSecondary, &m_numberOfSupply);
    public:
        ///> type=int
        int m_numberOfPrimary;

        ///> type=int
        int m_numberOfSecondary;

        ///> type=int
        int m_numberOfSupply;

        void AddEntity(GameEntity *p_entity);
        void RemoveEntity(GameEntity *p_entity);
        void Clear();
        float GetDistance(ResourceDescription *p_other) const;
    };
}

#endif // RESOURCEDESCRIPTION_H
