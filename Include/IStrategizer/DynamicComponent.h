#ifndef DYNAMICCOMPONENT_H
#define DYNAMICCOMPONENT_H

namespace IStrategizer
{
    const int DefaultInvalidationInterval = 50;

    class DynamicComponent
    {
    protected:
        unsigned		m_lastUpdate;
        unsigned		m_invalidationInterval;

        static bool		m_realTime;
        static unsigned	m_globalInvalidationInterval;
        static bool		m_globalInvalidation;

    public:
                    DynamicComponent() : m_invalidationInterval(0), m_lastUpdate(0) {}
                    DynamicComponent(unsigned p_invalidationInterval) : m_invalidationInterval(p_invalidationInterval), m_lastUpdate(0) {}
        virtual     ~DynamicComponent();
        unsigned    InvalidationInterval() { return m_invalidationInterval; }
        void        InvalidationInterval(unsigned p_newInterval) { m_invalidationInterval = p_newInterval; }
        bool        IsInvalid();

        static bool RealTime() { return m_realTime; }
        static void RealTime(bool p_enable) { m_realTime = p_enable; }
        static int  GlobalInvalidationInterval() { return m_globalInvalidationInterval; }
        static void GlobalInvalidationInterval(int val) { m_globalInvalidationInterval = val; }
        static bool GlobalInvalidation() { return m_globalInvalidation; }
        static void GlobalInvalidation(bool val) { m_globalInvalidation = val; }
    };
}

#endif // DYNAMICCOMPONENT_H