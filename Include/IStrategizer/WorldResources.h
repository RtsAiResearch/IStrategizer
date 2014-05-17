#ifndef WORLDRESOURCES_H
#define WORLDRESOURCES_H

#ifndef SHAREDRESOURCE_H
#include "SharedResource.h"
#endif

namespace IStrategizer
{
    class WorldResources : public SharedResource
    {
    public:
        static WorldResources FromEntity(int p_entityOrResearchType);
        WorldResources() : m_supply(0), m_secondary(0), m_primary(0) { }
        WorldResources(int p_supply, int p_secondary, int p_primary) : m_supply(p_supply), m_secondary(p_secondary), m_primary(p_primary) { }
        void Set(const WorldResources* pOther);
        void Supply (int supply) { m_supply = supply; }
        void Secondary (int secondary) { m_secondary = secondary; }
        void Primary (int primary) { m_primary = primary; }
        bool IsNull();
        virtual ~WorldResources() {}
        virtual int Supply() const { return m_supply; }
        virtual int Secondary() const { return m_secondary; }
        virtual int Primary() const { return m_primary; }

    protected:
        bool Acquire();
        bool Release();

        int m_supply;
        int m_secondary;
        int m_primary;
    };
}

#endif // WORLDRESOURCES_H
