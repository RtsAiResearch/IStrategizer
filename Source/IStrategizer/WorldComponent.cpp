#ifndef WORLDCOMPONENT_H
#include "WorldComponent.h"
#endif
using namespace IStrategizer;

#ifndef WORLDCLOCK_H
#include "WorldClock.h"
#endif

void WorldComponent::Update(const WorldClock& p_clock)
{
    m_lastUpdate = p_clock.ElapsedEngineCycles();
    UpdateAux(p_clock);
}
