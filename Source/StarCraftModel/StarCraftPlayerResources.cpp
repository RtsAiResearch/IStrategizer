#ifndef WARGUSPLAYERRESOURCES_H
#include "StarCraftPlayerResources.h"
#endif

#include "BWAPI.h"

using namespace StarCraftModel;
using namespace BWAPI;

int StarCraftPlayerResources::Supply() 
{ 
    return m_pPlayer->supplyTotal();
}
//////////////////////////////////////////////////////////////////////////
int StarCraftPlayerResources::Secondary()
{
    return m_pPlayer->gas();
}
//////////////////////////////////////////////////////////////////////////
int StarCraftPlayerResources::Primary()
{
    return m_pPlayer->minerals();
}
//////////////////////////////////////////////////////////////////////////
IClonable* StarCraftPlayerResources::Clone()
{
    StarCraftPlayerResources* pClone = new StarCraftPlayerResources();
    Copy(pClone);

    return pClone;
}
//////////////////////////////////////////////////////////////////////////
void StarCraftPlayerResources::Copy(IClonable* pDest)
{
    StarCraftPlayerResources* pConDest = dynamic_cast<StarCraftPlayerResources*>(pDest);
    _ASSERTE(pConDest);

    PlayerResources::Copy(pDest);
    pConDest->m_pPlayer = m_pPlayer;
}