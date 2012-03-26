#ifndef STARCRAFTTYPE_H
#include "StarCraftType.h"
#endif
#ifndef DEFINITIONCROSSMAPPING_H
#include "DefinitionCrossMapping.h"
#endif
#ifndef WORLDRESOURCES_H
#include "WorldResources.h"
#endif

#include "bwapi/UnitType.h"
#include "bwapi/WeaponType.h"

using namespace IStrategizer;
using namespace StarCraftModel;
using namespace BWAPI;

void StarCraftType::InitializeAux()
{
	// Positions are measured in pixels and are the highest resolution
	// Walk Tiles - each walk tile is an 8x8 square of pixels. These are called walk tiles because walkability data is available at this resolution.
	// Build Tiles - each build tile is a 4x4 square of walk tiles, or a 32x32 square of pixels.
	// These are called build tiles because buildability data is available at this resolution, and correspond to the tiles seen in game.
	// For example, a Command Center occupies an area of 4x3 build tiles. Build tiles can be specified in TilePosition objects.
	// We will use walk tiles as a measure of positions units across IStrategizer
	// See: http://code.google.com/p/bwapi/wiki/Misc

    m_id                = g_Database.EntityMapping.GetByFirst(m_type.getID());
    m_requiredResources = new WorldResources(m_type.supplyRequired(), m_type.gasPrice(), m_type.mineralPrice());

    Attr(ECATTR_CanAttack, m_type.canAttack());
    Attr(ECATTR_CanBuild, m_type.isWorker());
    Attr(ECATTR_IsBuilding, m_type.isBuilding());
    Attr(ECATTR_IsCowrad, m_type.isWorker());
    Attr(ECATTR_MaxHp, m_type.maxHitPoints());
	Attr(ECATTR_IsPrimaryResource, m_type.isMineralField());
	// FIXME: consider zerg and protoss refineries
	Attr(ECATTR_IsSecondaryResource, (m_type.getID() == UnitTypes::Resource_Vespene_Geyser.getID()));
	Attr(ECATTR_IsSupply, (m_type.supplyProvided() > 0));
	// FIXME: mention critical buildings
	Attr(ECATTR_IsCritical, 0);
	Attr(ECATTR_LineOfSight, m_type.sightRange());

	// Convert build tiles to walk tiles
	if (m_type.isBuilding())
	{
		Attr(ECATTR_Width, m_type.tileWidth() * 32);
		Attr(ECATTR_Height, m_type.tileHeight() * 32);
	}
	else
	{
		Attr(ECATTR_Width, m_type.dimensionLeft() * 2);
		Attr(ECATTR_Height, m_type.dimensionUp() * 2);
	}

	WeaponType	groundWeapon;
	WeaponType	airWeapon;
	int			groundDmg;
	int			airDmg;
	int			totalDmg;

	groundWeapon	= m_type.groundWeapon();
	airWeapon		= m_type.airWeapon();
	groundDmg		= groundWeapon.damageAmount();
	airDmg			= airWeapon.damageAmount();

	totalDmg = (airDmg + 1) * (groundDmg + 1);

	Attr(ECATTR_AirRange, airWeapon.maxRange());
	Attr(ECATTR_GroundRange, groundWeapon.maxRange());
	Attr(ECATTR_Attack, totalDmg);
}
