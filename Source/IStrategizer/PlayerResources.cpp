#include "PlayerResources.h"

using namespace IStrategizer;

bool PlayerResources::HasEnough(const WorldResources* p_resources)
{
    return this->Primary() >= p_resources->Primary() &&
        this->Secondary() >= p_resources->Secondary() &&
        this->Supply() >= p_resources->Supply();
}
//////////////////////////////////////////////////////////////////////////
bool PlayerResources::Lock(WorldResources* resources)
{
    bool amountAvailable = HasEnough(resources);

    if (amountAvailable)
    {
        if (resources->Supply() > 0)
        {
            _lockedSupply += resources->Supply();
        }

        if (resources->Secondary() > 0)
        {
            _lockedSecondary += resources->Secondary();
        }

        if (resources->Primary() > 0)
        {
            _lockedPrimary += resources->Primary();
        }
    }
    
    return amountAvailable;
}
//////////////////////////////////////////////////////////////////////////
void PlayerResources::Unlock(WorldResources* resources)
{
    if (resources->Supply() > 0)
    {
        _lockedSupply -= resources->Supply();
    }

    if (resources->Secondary() > 0)
    {
        _lockedSecondary -= resources->Secondary();
    }

    if (resources->Primary() > 0)
    {
        _lockedPrimary -= resources->Primary();
    }
}