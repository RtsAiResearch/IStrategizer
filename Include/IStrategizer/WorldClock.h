#ifndef WORLDCLOCK_H
#define WORLDCLOCK_H

namespace IStrategizer
{
    class WorldClock
    {
        int m_lastTick;
    protected:
        WorldClock() : m_lastTick(0) {}
    public:
        void        Tick(unsigned p_gameCycle) { m_lastTick = p_gameCycle; }
        unsigned    ElapsedCycles() const { return m_lastTick; }
        static WorldClock& Instance() { static WorldClock instance; return instance; }
    };

#define g_WorldClock WorldClock::Instance()

}

#endif // WORLDCLOCK_H