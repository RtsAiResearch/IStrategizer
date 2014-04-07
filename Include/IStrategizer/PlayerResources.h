#ifndef PLAYERRESOURCES_H
#define PLAYERRESOURCES_H

#ifndef WORLDCOMPONENT_H
#include "WorldComponent.h"
#endif
#ifndef WORLDRESOURCES_H
#include "WorldResources.h"
#endif

namespace IStrategizer
{
    class PlayerResources : public IClonable
    {
    public:
        bool Lock(WorldResources* pResources);
        void Unlock(WorldResources* pResources);
        int AvailableSupply() { return (Supply() - m_lockedSupply); }
        int AvailablePrimary() { return (Primary() - m_lockedPrimary); }
        int AvailableSecondary() { return (Secondary() - m_lockedSecondary); }
        PlayerResources() : m_lockedPrimary(0), m_lockedSecondary(0), m_lockedSupply(0) { }
        virtual ~PlayerResources() {}
        virtual bool HasEnough(const WorldResources* pResources);
        virtual int Supply() = 0;
        virtual int Secondary() = 0;
        virtual int Primary() = 0;
        void Copy(IClonable* pDest) const;

    protected:
        int m_lockedSupply;
        int m_lockedSecondary;
        int m_lockedPrimary;
    };
}

#endif // PLAYERRESOURCES_H
