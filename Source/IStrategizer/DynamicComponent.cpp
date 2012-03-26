#ifndef DYNAMICCOMPONENT_H
#include "DynamicComponent.h"
#endif

#ifndef WORLDCLOCK_H
#include "WorldClock.h"
#endif

using namespace IStrategizer;

bool DynamicComponent::m_realTime = false;
bool DynamicComponent::m_globalInvalidation = false;
unsigned DynamicComponent::m_globalInvalidationInterval = DefaultInvalidationInterval;

bool DynamicComponent::IsInvalid()
{
    if(m_realTime)
        return true;
    else
    {
        if(m_globalInvalidation)
            return g_WorldClock.ElapsedCycles() - m_lastUpdate > m_globalInvalidationInterval; 
        else
            return g_WorldClock.ElapsedCycles() - m_lastUpdate > m_invalidationInterval; 
    }
}
//----------------------------------------------------------------------------------------------
DynamicComponent::~DynamicComponent()
{

}