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
    class PlayerResources
    {
    public:
        bool Lock(WorldResources* resources);
        void Unlock(WorldResources* resources);
        int AvailableSupply() { return (Supply() - _lockedSupply); }
        int AvailablePrimary() { return (Primary() - _lockedPrimary); }
        int AvailableSecondary() { return (Secondary() - _lockedSecondary); }
        PlayerResources() : _lockedPrimary(0), _lockedSecondary(0), _lockedSupply(0) { }
        virtual ~PlayerResources() {}
        virtual bool HasEnough(const WorldResources* p_resources);
        virtual int Supply() = 0;
        virtual int Secondary() = 0;
        virtual int Primary() = 0;

    private:
        int _lockedSupply;
        int _lockedSecondary;
        int _lockedPrimary;
    };
}

#endif // PLAYERRESOURCES_H
