#include "WorldComponent.h"

#ifndef WORLDCLOCK_H
#include "WorldClock.h"
#endif

using namespace IStrategizer;

void WorldComponent::Update(const WorldClock& p_clock)
{
    m_lastUpdate = p_clock.ElapsedEngineCycles();
    UpdateAux(g_Game, p_clock);
}
