#ifndef STARCRAFTENTITY_H
#include "StarCraftEntity.h"
#endif
#ifndef METADATA_H
#include "MetaData.h"
#endif
#ifndef DEFINITIONCROSSMAPPING_H
#include "DefinitionCrossMapping.h"
#endif
#ifndef ERRORCODE_H
#include "ErrorCode.h"
#endif

using namespace StarCraftModel;
using namespace MetaData;
using namespace BWAPI;

#include "RtsGame.h"
#include "bwapi/Unit.h"
#include "bwapi/UnitType.h"
#include "bwapi/TilePosition.h"
#include "bwapi/Game.h"

#define TilePositionFromUnitPosition(UnitPos)	(UnitPos / 32)
#define UnitPositionFromTilePosition(TilePos)	(TilePos * 32)

StarCraftEntity::StarCraftEntity(Unit *p_unit) : GameEntity(p_unit->getID()), m_unit(p_unit)
{
	m_ownerId = g_Database.PlayerMapping.GetByFirst(m_unit->getPlayer()->getID());
	m_type = g_Database.EntityMapping.GetByFirst(m_unit->getType().getID());
}
//----------------------------------------------------------------------------------------------
int	StarCraftEntity::Attr(EntityObjectAttribute p_attrId)
{
	// Positions are measured in pixels and are the highest resolution
	// Walk Tiles - each walk tile is an 8x8 square of pixels. These are called walk tiles because walkability data is available at this resolution.
	// Build Tiles - each build tile is a 4x4 square of walk tiles, or a 32x32 square of pixels.
	// These are called build tiles because buildability data is available at this resolution, and correspond to the tiles seen in game.
	// For example, a Command Center occupies an area of 4x3 build tiles. Build tiles can be specified in TilePosition objects.
	// We will use walk tiles as a measure of positions units across IStrategizer
	// See: http://code.google.com/p/bwapi/wiki/Misc

    switch(p_attrId)
	{
	case EOATTR_Health:
		return m_unit->getHitPoints();

	case EOATTR_PosX:
		if (m_unit->getType().isBuilding())
			return UnitPositionFromTilePosition(m_unit->getTilePosition().x());
		else
			return m_unit->getLeft();

	case EOATTR_PosY:
		if (m_unit->getType().isBuilding())
			return UnitPositionFromTilePosition(m_unit->getTilePosition().y());
		else
			return m_unit->getTop();

	case EOATTR_State:
		return FetchState();
		
	case EOATTR_OwnerId:
		return m_ownerId;

	case EOATTR_PosCenterX:
		return m_unit->getPosition().x();

	case EOATTR_PosCenterY:
		return m_unit->getPosition().y();

	default:
		assert(0);
	}

	return 0;
}
//----------------------------------------------------------------------------------------------
ObjectStateType StarCraftEntity::FetchState()
{
    if (m_unit->isIdle() && m_unit->isCompleted())
        return OBJSTATE_Idle;
    else if (m_unit->isBeingConstructed() ||
		(m_unit->isIdle() && !m_unit->isCompleted()))
		return OBJSTATE_BeingConstructed;
	else if (m_unit->isGatheringGas() || m_unit->isGatheringMinerals())
		return OBJSTATE_Gathering;
	else if (m_unit->isMoving())
		return OBJSTATE_Moving;
	else if (m_unit->isConstructing())
		return OBJSTATE_Constructing;

    return OBJSTATE_END;
}
//----------------------------------------------------------------------------------------------
int	StarCraftEntity::Research(ResearchType p_researchId)
{
	Unit*	building = m_unit;
	bool	bOk;

	// Is tech
	if ((int)p_researchId >= TechIdOffset)
	{
		bOk = m_unit->research(g_Database.TechMapping.GetBySecond(p_researchId));
	}
	// Is upgrade
	else
	{
		bOk = m_unit->upgrade(g_Database.TechMapping.GetBySecond(p_researchId));
	}

	return bOk ? ERR_Success : ERR_InvalidParameterValue;
}
//----------------------------------------------------------------------------------------------
int	StarCraftEntity::Build(EntityClassType p_buildingClassId, int p_x, int p_y) 
{
	Unit*			builder = m_unit;
	TilePosition	pos(TilePositionFromUnitPosition(p_x), TilePositionFromUnitPosition(p_y));
	BWAPI::UnitType	type;
	TID				gameTypeId;
	string			typeName;

	if (g_Database.EntityMapping.ContainsSecond(p_buildingClassId))
		gameTypeId = g_Database.EntityMapping.GetBySecond(p_buildingClassId);
	else
		return ERR_InvalidParameterValue;

	if (g_Database.EntityIdentMapping.ContainsFirst(gameTypeId))
		typeName = g_Database.EntityIdentMapping.GetByFirst(gameTypeId);
	else
		return ERR_InvalidParameterValue;

	type = BWAPI::UnitTypes::getUnitType(typeName);

	return m_unit->build(pos, type) ? ERR_Success : ERR_InvalidParameterValue;
};
//----------------------------------------------------------------------------------------------
int	StarCraftEntity::AttackGround(int p_x, int p_y)
{
	bool		bOk;
	Unit*		attacker = m_unit;
	Position	pos(p_x, p_y);

	bOk = attacker->attack(pos);

	return bOk ? ERR_Success : ERR_InvalidParameterValue;
};
//----------------------------------------------------------------------------------------------
int	StarCraftEntity::AttackEntity(MetaData::PlayerType p_opponentIndex, int p_targetEntityObjectId)
{
	bool	bOk;
	Unit*	attacker = m_unit;
	Unit*	target;

	target = Broodwar->getUnit(p_targetEntityObjectId);

	if (!target)
		return ERR_InvalidParameterValue;

	bOk = attacker->attack(target);

	return bOk ? ERR_Success : ERR_InvalidParameterValue;
};
//----------------------------------------------------------------------------------------------
int	StarCraftEntity::Train(EntityClassType p_entityClassId)
{
	bool		bOk;
	Unit*		building = m_unit;
	TID	unitTypeId;
	string		typeName;
	BWAPI::UnitType	type;

	unitTypeId = g_Database.EntityMapping.GetBySecond(p_entityClassId);
	typeName = g_Database.EntityIdentMapping.GetByFirst(unitTypeId);

	type = BWAPI::UnitTypes::getUnitType(typeName);

	bOk = building->train(type);

	return bOk ? ERR_Success : ERR_InvalidParameterValue;
};