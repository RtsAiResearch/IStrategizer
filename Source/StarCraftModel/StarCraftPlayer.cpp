#ifndef STARCRAFTPLAYER_H
#include "StarCraftPlayer.h"
#endif
#ifndef STARCRAFTENTITY_H
#include "StarCraftEntity.h"
#endif
#ifndef DEFINITIONCROSSMAPPING_H
#include "DefinitionCrossMapping.h"
#endif
#ifndef WARGUSPLAYERRESOURCES_H
#include "StarCraftPlayerResources.h"
#endif
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif
#ifndef STARCRAFTTECHTREE_H
#include "StarCraftTechTree.h"
#endif

using namespace IStrategizer;
using namespace StarCraftModel;

StarCraftPlayer::StarCraftPlayer(Player p_pPlayer)  : m_pPlayer(p_pPlayer)
{
    m_id = g_Database.PlayerMapping.GetByFirst( p_pPlayer->getID());
    m_pResources = new StarCraftPlayerResources(m_pPlayer);
    m_pTechTree = new StarCraftTechTree(m_pPlayer);
}
//----------------------------------------------------------------------------------------------
GameEntity* StarCraftPlayer::FetchEntity(TID p_id)
{
    BWAPI::Unit pUnit = Broodwar->getUnit(p_id);

    if (pUnit)
        return new StarCraftEntity(pUnit);
    else
        return nullptr;
}
//----------------------------------------------------------------------------------------------
void StarCraftPlayer::Copy(IClonable* pDest)
{
    StarCraftPlayer* pConDest = dynamic_cast<StarCraftPlayer*>(pDest);
    _ASSERTE(pConDest);

    GamePlayer::Copy(pDest);
    pConDest->m_pPlayer = m_pPlayer;
}
//----------------------------------------------------------------------------------------------
IClonable* StarCraftPlayer::Clone()
{
    StarCraftPlayer* pClone = new StarCraftPlayer();
    Copy(pClone);

    return pClone;
}
