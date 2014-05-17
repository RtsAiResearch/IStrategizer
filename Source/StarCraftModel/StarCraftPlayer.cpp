#ifndef STARCRAFTPLAYER_H
#include "StarCraftPlayer.h"
#endif
#ifndef STARCRAFTENTITY_H
#include "StarCraftEntity.h"
#endif
#ifndef DEFINITIONCROSSMAPPING_H
#include "DefinitionCrossMapping.h"
#endif
#include "StarCraftPlayerResources.h"
#ifndef TOOLBOX_H
#include "Toolbox.h"
#endif
#ifndef STARCRAFTTECHTREE_H
#include "StarCraftTechTree.h"
#endif
#include "StarCraftRace.h"

using namespace IStrategizer;

StarCraftPlayer::StarCraftPlayer(Player p_pPlayer)  : m_pPlayer(p_pPlayer)
{
    m_id = g_Database.PlayerMapping.GetByFirst( p_pPlayer->getID());
    m_pResources = new StarCraftPlayerResources(m_pPlayer);
    m_pTechTree = new StarCraftTechTree(m_pPlayer);
    m_pRace = new StarCraftRace(m_pPlayer);
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