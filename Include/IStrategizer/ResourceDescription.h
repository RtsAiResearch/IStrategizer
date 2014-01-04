///> [Serializable]
#ifndef RESOURCEDESCRIPTION_H
#define RESOURCEDESCRIPTION_H

#ifndef SVECTOR_H
#include "SVector.h"
#endif

#include "MetaData.h"
#include "UserObject.h"

namespace IStrategizer
{
    class GameEntity;

    ///> class=ResourceDescription
    class ResourceDescription : public Serialization::UserObject
    {
        OBJECT_SERIALIZABLE(ResourceDescription);

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
        float GetDistance(ResourceDescription *p_other);

    protected:
        void InitializeAddressesAux();
    };
}

#endif // RESOURCEDESCRIPTION_H
