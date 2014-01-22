#include "WorldComponent.h"
#include "WorldClock.h"

using namespace IStrategizer;

void WorldComponent::Update(RtsGame& p_RtsGame, const WorldClock& p_clock)
{
    m_lastUpdate = p_clock.ElapsedEngineCycles();
    UpdateAux(p_RtsGame, p_clock);
}
