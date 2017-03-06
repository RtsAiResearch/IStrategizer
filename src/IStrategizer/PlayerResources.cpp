#include "PlayerResources.h"
#include "Action.h"
#include "Logger.h"
#include "ObjectFactory.h"

using namespace IStrategizer;

DECL_SERIALIZABLE(PlayerResources);

bool PlayerResources::HasEnough(const WorldResources* pResources)
{
    int availablePrimary = this->AvailablePrimary();
    int availableSecondary = this->AvailableSecondary();
    int availableSupply = this->AvailableSupply();

    int requiredPrimary = pResources->Primary();
    int requiredSecondary = pResources->Secondary();
    int requiredSupply = pResources->Supply();

    return (requiredPrimary == 0 || availablePrimary >= requiredPrimary) &&
        (requiredSecondary == 0 || availableSecondary >= requiredSecondary) &&
        (requiredSupply == 0 || availableSupply >= requiredSupply);
}
//////////////////////////////////////////////////////////////////////////
bool PlayerResources::Lock(WorldResources* pResources)
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

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
            m_lockedSupply, m_lockedPrimary, m_lockedSecondary, AvailablePrimary(), AvailableSecondary(), AvailableSupply());
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
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

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
//////////////////////////////////////////////////////////////////////////
int PlayerResources::Supply() const
{
    if (m_isOnline)
    {
        // The amount of supply is doubled because of an issue with the Zerg supply
        // that's why we divide the supplyTotal over 2.
        // For more info check documentation for supplyTotal API.
        int bwapiTotalSupply = g_GameImpl->PlayerSupplyTotal(m_playerId);
        int bwapiUsedSupply = g_GameImpl->PlayerSupplyUsed(m_playerId);
        int totalSupply = bwapiTotalSupply / 2;
        int usedSupply = (bwapiUsedSupply / 2);
        return totalSupply - usedSupply;
    }
    else
        return m_cachedSupply;
}
//////////////////////////////////////////////////////////////////////////
int PlayerResources::Secondary() const
{
    if (m_isOnline)
        return g_GameImpl->PlayerGas(m_playerId);
    else
        return m_cachedSecondary;
}
//////////////////////////////////////////////////////////////////////////
int PlayerResources::Primary() const
{
    if (m_isOnline)
        return g_GameImpl->PlayerMinerals(m_playerId);
    else
        return m_cachedPrimary;
}

