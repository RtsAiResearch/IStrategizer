#include "PlayerResources.h"

using namespace IStrategizer;

bool PlayerResources::HasEnough(const WorldResources* pResources)
{
    return this->Primary() >= pResources->Primary() &&
        this->Secondary() >= pResources->Secondary() &&
        this->Supply() >= pResources->Supply();
}
//////////////////////////////////////////////////////////////////////////
bool PlayerResources::Lock(WorldResources* pResources)
{
    bool amountAvailable = HasEnough(pResources);

    if (amountAvailable)
    {
        if (pResources->Supply() > 0)
        {
            m_lockedSupply += pResources->Supply();
        }

        if (pResources->Secondary() > 0)
        {
            m_lockedSecondary += pResources->Secondary();
        }

        if (pResources->Primary() > 0)
        {
            m_lockedPrimary += pResources->Primary();
        }
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
void PlayerResources::Copy(IClonable* pDest) const
{
    PlayerResources* pConDest = dynamic_cast<PlayerResources*>(pDest);
    _ASSERTE(pConDest);

    pConDest->m_lockedSupply = m_lockedSupply;
    pConDest->m_lockedSecondary = m_lockedSecondary;
    pConDest->m_lockedPrimary = m_lockedPrimary;
}
