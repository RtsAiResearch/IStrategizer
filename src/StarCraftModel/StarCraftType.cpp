#ifndef STARCRAFTTYPE_H
#include "StarCraftType.h"
#endif
#ifndef DEFINITIONCROSSMAPPING_H
#include "DefinitionCrossMapping.h"
#endif
#ifndef WORLDRESOURCES_H
#include "WorldResources.h"
#endif
#include "ObjectFactory.h"
#include <string>
#include "BWAPI.h"

using namespace IStrategizer;
using namespace BWAPI;
using namespace std;

DECL_SERIALIZABLE(StarCraftType);

void StarCraftType::Init()
{
    // Positions are measured in pixels and are the highest resolution
    // Walk Tiles - each walk tile is an 8x8 square of pixels. These are called walk tiles because walkability data is available at this resolution.
    // Build Tiles - each build tile is a 4x4 square of walk tiles, or a 32x32 square of pixels.
    // These are called build tiles because buildability data is available at this resolution, and correspond to the tiles seen in game.
    // For example, a Command Center occupies an area of 4x3 build tiles. Build tiles can be specified in TilePosition objects.
    // We will use walk tiles as a measure of positions units across IStrategizer
    // See: http://code.google.com/p/bwapi/wiki/Misc

    m_id                = g_Database.EntityMapping.GetByFirst(m_type.getID());
    // The supply amount is doubled, divide over two.
    m_requiredResources = WorldResources(m_type.supplyRequired() / 2, m_type.gasPrice(), m_type.mineralPrice());

    P(TP_CanAttack, m_type.canAttack());
    P(TP_CanBuild, m_type.canBuildAddon() || m_type.isWorker());
    P(TP_IsBuilding, m_type.isBuilding());
    P(TP_IsProducer, m_type.canProduce());
    P(TP_IsWorker, m_type.isWorker());
    P(TP_IsAttacker, m_type.canAttack() && !m_type.isWorker());
    P(TP_MaxHp, m_type.maxHitPoints());
    P(TP_IsPrimaryResource, m_type.isMineralField());
    // FIXME: consider zerg and protoss refineries
    P(TP_IsSecondaryResource, (m_type.getID() == UnitTypes::Resource_Vespene_Geyser.getID()));
    P(TP_IsSupply, (m_type.supplyProvided() > 0));
    // FIXME: mention critical buildings
    P(TP_LineOfSight, m_type.sightRange());
	P(TP_BuildTime, m_type.buildTime());

    // Convert build tiles to walk tiles
    if (m_type.isBuilding())
    {
        P(TP_Width, m_type.tileWidth() * 32);
        P(TP_Height, m_type.tileHeight() * 32);
    }
    else
    {
        P(TP_Width, m_type.dimensionLeft() * 2);
        P(TP_Height, m_type.dimensionUp() * 2);
    }

    WeaponType groundWeapon = m_type.groundWeapon();
    WeaponType airWeapon = m_type.airWeapon();

    P(TP_AirRange, airWeapon.maxRange());
    P(TP_AirAttack, airWeapon.damageAmount());
    P(TP_GroundRange, groundWeapon.maxRange());
    P(TP_GroundAttack, groundWeapon.damageAmount());
    P(TP_IsSpecialBuilding, m_type.isRefinery() || m_type.isAddon());
    P(TP_IsMelee, groundWeapon.maxRange() <= TILE_SIZE);
}

EntityClassType StarCraftType::GetBuilderType() const
{
    if (m_type.isAddon())
    {
        return g_Database.EntityMapping.GetByFirst(m_type.whatBuilds().first.getID());
    }
    else
    {
        _ASSERTE(m_type.isBuilding());
        return g_Database.EntityMapping.GetByFirst(m_type.getRace().getWorker().getID());
    }
}

void StarCraftType::GetRequirements(vector<ResearchType>& researches, map<EntityClassType, unsigned>& buildings) const
{
    if (m_type.requiredTech().getID() != TechTypes::None.getID())
    {
        ResearchType requiredResearch = g_Database.TechMapping.GetByFirst(m_type.requiredTech().getID());
        researches.push_back(requiredResearch);
    }

    for (auto& requiredUnit : m_type.requiredUnits())
    {
        EntityClassType requiredEntity = g_Database.EntityMapping.GetByFirst(requiredUnit.first.getID());
        buildings[requiredEntity] = requiredUnit.second;
    }
}

EntityClassType StarCraftType::SourceEntity() const
{
    return g_Database.EntityMapping.GetByFirst(m_type.whatBuilds().first.getID());
}
