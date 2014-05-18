#include "PlayerResources.h"
#include "Action.h"
#include "Logger.h"

using namespace IStrategizer;

bool PlayerResources::HasEnough(const WorldResources* pResources)
{
    int availablePrimary = this->AvailablePrimary();
    int availableSecondary = this->AvailableSecondary();
    int availableSupply = this->AvailableSupply();

    int requiredPrimary = pResources->Primary();
    int requiredSecondary = pResources->Secondary();
    int requiredSupply = pResources->Supply();

    return availablePrimary >= requiredPrimary &&
           availableSecondary >= requiredSecondary &&
           availableSupply >= requiredSupply;
}
//////////////////////////////////////////////////////////////////////////
bool PlayerResources::Lock(WorldResources* pResources)
{
    bool amountAvailable = HasEnough(pResources);

    LogInfo("An action is trying to lock supply=%d, primary=%d, secondary=%d", pResources->Supply(), pResources->Primary(), pResources->Secondary());

    if (amountAvailable)
    {
        if (pResources->Supply() > 0)
        {
            _ASSERTE(m_lockedSupply + pResources->Supply() <= Supply());
            m_lockedSupply += pResources->Supply();
        }

        if (pResources->Secondary() > 0)
        {
            _ASSERTE(m_lockedSecondary + pResources->Secondary() <= Secondary());
            m_lockedSecondary += pResources->Secondary();
        }

        if (pResources->Primary() > 0)
        {
            _ASSERTE(m_lockedPrimary + pResources->Primary() <= Primary());
            m_lockedPrimary += pResources->Primary();
        }

        LogInfo("Action succeeded to lock requested resources");
        LogInfo("Total locked supply=%d, primary=%d, secondary=%d | Available supply=%d, primary=%d, secondary=%d",
            m_lockedSupply, m_lockedPrimary, m_lockedSecondary, Supply(), Primary(), Secondary());
    }
    else
    {
        LogInfo("Action failed to lock requested resources");
    }
    
    return amountAvailable;
}
//////////////////////////////////////////////////////////////////////////
void PlayerResources::Unlock(WorldResources* pResources)
{
    if (pResources->Supply() > 0)
    {
        m_lockedSupply -= pResources->Supply();
    }

    if (pResources->Secondary() > 0)
    {
        m_lockedSecondary -= pResources->Secondary();
    }

    if (pResources->Primary() > 0)
    {
        m_lockedPrimary -= pResources->Primary();
    }
}
//////////////////////////////////////////////////////////////////////////
void PlayerResources::SetOffline(RtsGame* pBelongingGame)
{
    _ASSERTE(m_isOnline);

    m_cachedPrimary = Primary();
    m_cachedSecondary = Secondary();
    m_cachedSupply = Supply();

    m_isOnline = false;
}