#include "PlayerResources.h"
#include "Action.h"
#include "Logger.h"

using namespace IStrategizer;

bool PlayerResources::HasEnough(const WorldResources* p_resources)
{
    int availablePrimary = this->AvailablePrimary();
    int availableSecondary = this->AvailableSecondary();
    int availableSupply = this->AvailableSupply();

    int requiredPrimary = p_resources->Primary();
    int requiredSecondary = p_resources->Secondary();
    int requiredSupply = p_resources->Supply();

    return availablePrimary >= requiredPrimary &&
           availableSecondary >= requiredSecondary &&
           availableSupply >= requiredSupply;
}
//////////////////////////////////////////////////////////////////////////
bool PlayerResources::Lock(WorldResources* resources)
{
    bool amountAvailable = HasEnough(resources);

    LogInfo("An action is trying to lock supply=%d, primary=%d, secondary=%d", resources->Supply(), resources->Primary(), resources->Secondary());

    if (amountAvailable)
    {
        if (resources->Supply() > 0)
        {
            _ASSERTE(_lockedSupply + resources->Supply() <= Supply());
            _lockedSupply += resources->Supply();
        }

        if (resources->Secondary() > 0)
        {
            _ASSERTE(_lockedSecondary + resources->Secondary() <= Secondary());
            _lockedSecondary += resources->Secondary();
        }

        if (resources->Primary() > 0)
        {
            _ASSERTE(_lockedPrimary + resources->Primary() <= Primary());
            _lockedPrimary += resources->Primary();
        }

        LogInfo("Action succeeded to lock requested resources");
        LogInfo("Total locked supply=%d, primary=%d, secondary=%d | Available supply=%d, primary=%d, secondary=%d",
            _lockedSupply, _lockedPrimary, _lockedSecondary, Supply(), Primary(), Secondary());
    }
    else
    {
        LogInfo("Action failed to lock requested resources");
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