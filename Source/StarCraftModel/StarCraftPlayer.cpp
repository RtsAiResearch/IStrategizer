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
#include "ObjectFactory.h"
#include "StarCraftRace.h"
#include "RtsGame.h"
#include "BWAPI.h"

using namespace IStrategizer;
using namespace BWAPI;

DECL_SERIALIZABLE(StarCraftPlayer);

StarCraftPlayer::StarCraftPlayer(Player pPlayer) :
    GamePlayer(pPlayer->getRace().getID()),
    m_pPlayer(pPlayer)
{
    m_type = g_Database.PlayerMapping.GetByFirst(pPlayer->getID());
    m_pResources = new StarCraftPlayerResources(m_pPlayer);
    m_pTechTree = new StarCraftTechTree(m_pPlayer);
}

const GameRace* StarCraftPlayer::Race() const
{
    return g_Game->GetRace(m_raceId);
}

GameEntity* StarCraftPlayer::FetchEntity(TID id)
{
    BWAPI::Unit pUnit = Broodwar->getUnit(id);
    _ASSERTE(pUnit);

    return new StarCraftEntity(pUnit);
}

void StarCraftPlayer::SetOffline(RtsGame* pBelongingGame)
{
    for (auto entityEntry : m_entities)
        Toolbox::MemoryClean(entityEntry.second);
    m_entities.clear();

    for (auto pUnit : m_pPlayer->getUnits())
    {
        m_entities[pUnit->getID()] = new StarCraftEntity(pUnit);
        m_entities[pUnit->getID()]->SetOffline(pBelongingGame);
    }

    GamePlayer::SetOffline(pBelongingGame);
}
