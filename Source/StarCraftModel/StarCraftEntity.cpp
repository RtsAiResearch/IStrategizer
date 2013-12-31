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
#include "IStrategizerException.h"
#include "MathHelper.h"
#include "RtsGame.h"

using namespace StarCraftModel;
using namespace IStrategizer;
using namespace BWAPI;
using namespace std;

#define TilePositionFromUnitPosition(UnitPos)	(UnitPos / 32)
#define UnitPositionFromTilePosition(TilePos)	(TilePos * 32)

StarCraftEntity::StarCraftEntity(Unit p_unit) : GameEntity(p_unit->getID()), m_unit(p_unit)
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
			return UnitPositionFromTilePosition(m_unit->getTilePosition().x);
		else
			return m_unit->getLeft();

	case EOATTR_PosY:
		if (m_unit->getType().isBuilding())
			return UnitPositionFromTilePosition(m_unit->getTilePosition().y);
		else
			return m_unit->getTop();

	case EOATTR_State:
		return FetchState();

	case EOATTR_OwnerId:
		return m_ownerId;

	case EOATTR_PosCenterX:
		return m_unit->getPosition().x;

	case EOATTR_PosCenterY:
		return m_unit->getPosition().y;

	default:
		assert(0);
	}

	return 0;
}
//----------------------------------------------------------------------------------------------
ObjectStateType StarCraftEntity::FetchState()
{
	bool isIdle = m_unit->isIdle();
	bool isCompleted = m_unit->isCompleted();
	bool isBeingConstructed =m_unit->isBeingConstructed();
	bool isConstructing = m_unit->isConstructing();
	bool isMoving = m_unit->isMoving();
	bool isGatheringGas = m_unit->isGatheringGas();
	bool isGatheringMinerals = m_unit->isGatheringMinerals();
	bool isTraining = m_unit->isTraining();

	if (isIdle && isCompleted)
		return OBJSTATE_Idle;
	else if (isBeingConstructed ||
		(isIdle && !isCompleted))
		return OBJSTATE_BeingConstructed;
	else if (isGatheringGas || isGatheringMinerals)
		return OBJSTATE_Gathering;
	else if (isConstructing)
		return OBJSTATE_Constructing;
	else if (isMoving)
		return OBJSTATE_Moving;
	else if (isTraining)
		return OBJSTATE_Training;
	else
		return OBJSTATE_END;
}
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::Research(ResearchType p_researchId)
{
	bool	bOk;

	// Is tech
	if ((int)p_researchId >= ((int)(RESEARCH_START +  TechIdOffset)))
	{
		TID gameTypeID = g_Database.TechMapping.GetBySecond(p_researchId);

		bOk = m_unit->research(TechType(gameTypeID));
	}
	// Is upgrade
	else
	{
		TID gameTypeID = g_Database.UpgradeMapping.GetBySecond(p_researchId);
		bOk = m_unit->upgrade(UpgradeType(gameTypeID));
	}

	return bOk;
}
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::Build(EntityClassType p_buildingClassId, int p_x, int p_y) 
{
	TilePosition	  pos(TilePositionFromUnitPosition(p_x), TilePositionFromUnitPosition(p_y));
	BWAPI::UnitType	type;
	TID             gameTypeId;
	string	        typeName;

	gameTypeId = g_Database.EntityMapping.GetBySecond(p_buildingClassId);
	typeName = g_Database.EntityIdentMapping.GetByFirst(gameTypeId);

	type = BWAPI::UnitType::getType(typeName);

	return m_unit->build(type, pos);
};
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::AttackGround(int p_x, int p_y)
{
	Unit		attacker = m_unit;
	Position	pos(p_x, p_y);

	return attacker->attack(pos);
};
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::Move(int p_x, int p_y)
{
	Unit		unit = m_unit;
	Position	pos(p_x, p_y);

	return unit->move(pos);
};
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::AttackEntity(IStrategizer::PlayerType p_opponentIndex, int p_targetEntityObjectId)
{
	Unit	attacker = m_unit;
	Unit	target;

	target = Broodwar->getUnit(p_targetEntityObjectId);

	if (!target)
		throw ItemNotFoundException(XcptHere);

	return attacker->attack(target);
};
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::Train(EntityClassType p_entityClassId)
{
	Unit			building = m_unit;
	TID				unitTypeId;
	string			typeName;
	BWAPI::UnitType	type;

	unitTypeId = g_Database.EntityMapping.GetBySecond(p_entityClassId);
	typeName = g_Database.EntityIdentMapping.GetByFirst(unitTypeId);

	type = BWAPI::UnitType::getType(typeName);

	return building->train(type);

};
//----------------------------------------------------------------------------------------------
bool StarCraftModel::StarCraftEntity::IsTraining(TID p_traineeId)
{
	Unit traineeObj = Broodwar->getUnit(p_traineeId);

	if (nullptr == traineeObj)
		throw ItemNotFoundException(XcptHere);

	return MathHelper::RectangleMembership(
		m_unit->getLeft(),
		m_unit->getTop(),
		m_unit->getRight() - m_unit->getLeft(),
		m_unit->getBottom() - m_unit->getTop(),
		traineeObj->getPosition().x,
		traineeObj->getPosition().y);
}
//----------------------------------------------------------------------------------------------
string StarCraftModel::StarCraftEntity::ToString() const
{
	std::string asSharedResource = SharedResource::ToString();

	std::string description = m_unit->getType().getName();
	description += "(";
	description += m_unit->getID();
	description += ",";
	description += asSharedResource;
	description += ")";

	return description;
}
//----------------------------------------------------------------------------------------------
IStrategizer::Vector2 StarCraftEntity::GetPosition()
{
	return Vector2(m_unit->getPosition().x, m_unit->getPosition().y);
}