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

#include "bwapi/Player.h"
#include "bwapi/Unit.h"
#include "bwapi/Game.h"

using namespace IStrategizer;
using namespace StarCraftModel;

StarCraftPlayer::StarCraftPlayer(Player *p_pPlayer)  : m_pPlayer(p_pPlayer)
{
    m_id = g_Database.PlayerMapping.GetByFirst( p_pPlayer->getID());
    m_pResources = new StarCraftPlayerResources(m_pPlayer);
    m_pTechTree	= new StarCraftTechTree(m_pPlayer);

	TID typeId;
	
	typeId = m_pPlayer->getRace().getWorker().getID();
	m_workerTypeId = g_Database.EntityMapping.GetByFirst(typeId);

	typeId = m_pPlayer->getRace().getCenter().getID();
	m_baseTypeId = g_Database.EntityMapping.GetByFirst(typeId);
}
//----------------------------------------------------------------------------------------------
GameEntity* StarCraftPlayer::FetchEntity(TID p_id)
{
	BWAPI::Unit *pUnit = Broodwar->getUnit(p_id);

	if (pUnit)
		return new StarCraftEntity(pUnit);
	else
		return NULL;
}
//----------------------------------------------------------------------------------------------
const GameStateEx* StarCraftPlayer::State()
{
	// TODO: add StarCraft game state
	return GamePlayer::State();
}
