#ifndef WORLDCLOCK_H
#define WORLDCLOCK_H

namespace IStrategizer
{
    class WorldClock
    {
    public:
        WorldClock() 
            : m_timerStart(0),
            m_lastGameCycle(0),
            m_lastEngineCycle(0) {}
        unsigned    ElapsedGameCycles() const { return m_lastGameCycle; }
        unsigned ElapsedEngineCycles() const { return m_lastEngineCycle; }
        unsigned ElapsedMilliseconds() const { return m_elapsedMilliseconds; }
        void Reset();
        void Update(unsigned p_gameCycle);

    protected:
        void        GameTick(unsigned p_gameCycle) { m_lastGameCycle = p_gameCycle; }
        void EngineTick() { ++m_lastEngineCycle; }
        void CalculateElapsedMilliseconds();
    private:
        unsigned long long m_timerStart;
        unsigned m_lastGameCycle;
        unsigned m_lastEngineCycle;
        unsigned m_elapsedMilliseconds;
    };
}

#endif // WORLDCLOCK_H
