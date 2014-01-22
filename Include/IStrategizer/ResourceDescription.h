///> [Serializable]
#ifndef RESOURCEDESCRIPTION_H
#define RESOURCEDESCRIPTION_H

#include "SVector.h"
#include "MetaData.h"
#include "UserObject.h"
#include "RtsGame.h"

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

        void AddEntity(RtsGame& p_RtsGame, GameEntity *p_entity);
        void RemoveEntity(RtsGame& p_RtsGame, GameEntity *p_entity);
        void Clear();
        float GetDistance(ResourceDescription *p_other);

    protected:
        void InitializeAddressesAux();
    };
}

#endif // RESOURCEDESCRIPTION_H
