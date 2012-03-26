#ifndef WORLDCOMPONENT_H
#include "WorldComponent.h"
#endif
using namespace IStrategizer;

#ifndef WORLDCLOCK_H
#include "WorldClock.h"
#endif

void WorldComponent::Update()
{
    m_lastUpdate = g_WorldClock.ElapsedCycles();
    UpdateAux();
}
