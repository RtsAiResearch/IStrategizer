///> [Serializable]
#ifndef PLAYERRESOURCES_H
#define PLAYERRESOURCES_H

#ifndef WORLDRESOURCES_H
#include "WorldResources.h"
#endif
#include "UserObject.h"

namespace IStrategizer
{
    class RtsGame;

    ///> class=PlayerResources
    class PlayerResources : public Serialization::UserObject
    {
        OBJECT_MEMBERS(7, m_lockedPrimary, &m_lockedSecondary, &m_lockedSupply,
            &m_isOnline, &m_cachedPrimary, &m_cachedSecondary, &m_cachedSupply);

    public:
        PlayerResources() :
            m_lockedPrimary(0),
            m_lockedSecondary(0),
            m_lockedSupply(0),
            m_isOnline(true)
        { }

        bool Lock(WorldResources* resources);
        void Unlock(WorldResources* resources);
        int AvailableSupply() { return (Supply() - m_lockedSupply); }
        int AvailablePrimary() { return (Primary() - m_lockedPrimary); }
        int AvailableSecondary() { return (Secondary() - m_lockedSecondary); }

        virtual ~PlayerResources() {}
        virtual bool HasEnough(const WorldResources* p_resources);
        virtual int Supply() = 0;
        virtual int Secondary() = 0;
        virtual int Primary() = 0;
        void SetOffline(RtsGame* pBelongingGame);
        bool IsOnline() const { return m_isOnline; }

    protected:
        ///> type=int
        int m_lockedPrimary;
        ///> type=int
        int m_lockedSecondary;
        ///> type=int
        int m_lockedSupply;
        ///> type=bool
        bool m_isOnline;
        ///> type=int
        int m_cachedPrimary;
        ///> type=int
        int m_cachedSecondary;
        ///> type=int
        int m_cachedSupply;
    };
}

#endif // PLAYERRESOURCES_H
