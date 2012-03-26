#ifndef WORLDRESOURCES_H
#define WORLDRESOURCES_H

namespace IStrategizer
{
    class WorldResources
    {
    public:
		WorldResources() : m_supply(0), m_secondary(0), m_primary(0) {}
		WorldResources(int p_supply, int p_secondary, int p_primary) : m_supply(p_supply), m_secondary(p_secondary), m_primary(p_primary) {}
        virtual ~WorldResources() {}
        virtual int Supply() const { return m_supply; }
        virtual int Secondary() const { return m_secondary; }
        virtual int Primary() const { return m_primary; }

	protected:
		int m_supply;
		int m_secondary;
		int m_primary;
    };
}


#endif // WORLDRESOURCES_H