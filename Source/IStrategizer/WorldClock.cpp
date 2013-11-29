#include "WorldClock.h"
#include <cassert>
#include <Windows.h>

using namespace IStrategizer;

#define MillisecondFrom100Nano(T) ((T) / 10000)

void WorldClock::Reset()
{
	// Reset cycles
	m_lastEngineCycle = 0;
	m_lastGameCycle = 0;

	// Reset clock timer
	FILETIME ftTimerStart;
	ULARGE_INTEGER uliTimerStart;

	GetSystemTimeAsFileTime(&ftTimerStart);

	uliTimerStart.LowPart = ftTimerStart.dwLowDateTime;
	uliTimerStart.HighPart = ftTimerStart.dwHighDateTime;

	m_timerStart = uliTimerStart.QuadPart;
}
//////////////////////////////////////////////////////////////////////////
unsigned WorldClock::ElapsedMilliseconds()
{
	FILETIME ftTimerNow;
	ULARGE_INTEGER uliTimerNow;

	GetSystemTimeAsFileTime(&ftTimerNow);

	uliTimerNow.LowPart = ftTimerNow.dwLowDateTime;
	uliTimerNow.HighPart = ftTimerNow.dwHighDateTime;

	return (unsigned)MillisecondFrom100Nano(uliTimerNow.QuadPart - m_timerStart);
}