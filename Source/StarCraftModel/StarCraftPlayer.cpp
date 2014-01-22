#include "StarCraftPlayer.h"
#include "StarCraftEntity.h"
#include "DefinitionCrossMapping.h"
#include "StarCraftPlayerResources.h"
#include "Toolbox.h"
#include "StarCraftTechTree.h"

using namespace IStrategizer;
using namespace StarCraftModel;

StarCraftPlayer::StarCraftPlayer(RtsGame& p_RtsGame, Player p_pPlayer)
    : GamePlayer(p_RtsGame),
    m_pPlayer(p_pPlayer)
{
    m_id = g_Database.PlayerMapping.GetByFirst( p_pPlayer->getID());
    m_pResources = new StarCraftPlayerResources(m_pPlayer);
    m_pTechTree = new StarCraftTechTree(m_pPlayer);

    TID typeId;
    
    typeId = m_pPlayer->getRace().getWorker().getID();
    m_workerTypeId = g_Database.EntityMapping.GetByFirst(typeId);

    typeId = m_pPlayer->getRace().getCenter().getID();
    m_baseTypeId = g_Database.EntityMapping.GetByFirst(typeId);
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
const GameStateEx* StarCraftPlayer::State()
{
    // TODO: add StarCraft game state
    return GamePlayer::State();
}
