#include "PlayerResources.h"

using namespace IStrategizer;

bool PlayerResources::HasEnough(const WorldResources* p_resources)
{
    return this->Primary() >= p_resources->Primary() &&
        this->Secondary() >= p_resources->Secondary() &&
        this->Supply() >= p_resources->Supply();
}
//////////////////////////////////////////////////////////////////////////
void PlayerResources::Lock(ResourceType p_resourceId, int p_amount)
{
    switch(p_resourceId)
    {
    case RESOURCE_Supply:
        _lockedSupply += p_amount;
        break;
    case RESOURCE_Primary:
        _lockedPrimary += p_amount;
        break;
    case RESOURCE_Secondary:
        _lockedSecondary += p_amount;
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
void PlayerResources::Unlock(ResourceType p_resourceId, int p_amount)
{
    switch(p_resourceId)
    {
    case RESOURCE_Supply:
        _lockedSupply -= p_amount;
        break;
    case RESOURCE_Primary:
        _lockedPrimary -= p_amount;
        break;
    case RESOURCE_Secondary:
        _lockedSecondary -= p_amount;
        break;
    }
}