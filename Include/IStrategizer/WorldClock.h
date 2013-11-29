#ifndef WORLDCLOCK_H
#define WORLDCLOCK_H

namespace IStrategizer
{
    class WorldClock
    {
    public:
        void        GameTick(unsigned p_gameCycle) { m_lastGameCycle = p_gameCycle; }
		void		EngineTick() { ++m_lastEngineCycle; }
        unsigned    ElapsedGameCycles() const { return m_lastGameCycle; }
		unsigned	ElapsedEngineCycles() const { return m_lastEngineCycle; }
		unsigned	ElapsedMilliseconds();
		void		Reset();
        static WorldClock& Instance() { static WorldClock instance; return instance; }
	protected:
		WorldClock() : m_lastGameCycle(0), m_lastEngineCycle(0) {}
	private:
		unsigned long long	m_timerStart;
		unsigned			m_lastGameCycle;
		unsigned			m_lastEngineCycle;
    };

#define g_WorldClock WorldClock::Instance()

}

#endif // WORLDCLOCK_H