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

#define TilePositionFromUnitPosition(UnitPos) (UnitPos / 32)
#define UnitPositionFromTilePosition(TilePos) (TilePos * 32)

StarCraftEntity::StarCraftEntity(Unit p_unit) : GameEntity(p_unit->getID()), m_pUnit(p_unit)
{
    m_ownerId = g_Database.PlayerMapping.GetByFirst(m_pUnit->getPlayer()->getID());
    m_type = g_Database.EntityMapping.GetByFirst(m_pUnit->getType().getID());
}
//----------------------------------------------------------------------------------------------
int StarCraftEntity::FetchAttr(EntityObjectAttribute p_attrId) const
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
        return m_pUnit->getHitPoints();

    case EOATTR_PosX:
        if (m_pUnit->getType().isBuilding())
            return UnitPositionFromTilePosition(m_pUnit->getTilePosition().x);
        else
            return m_pUnit->getLeft();

    case EOATTR_PosY:
        if (m_pUnit->getType().isBuilding())
            return UnitPositionFromTilePosition(m_pUnit->getTilePosition().y);
        else
            return m_pUnit->getTop();

    case EOATTR_State:
        return FetchState();

    case EOATTR_OwnerId:
        return m_ownerId;

    case EOATTR_PosCenterX:
        return m_pUnit->getPosition().x;

    case EOATTR_PosCenterY:
        return m_pUnit->getPosition().y;

    case EOATTR_IsMoving:
        return m_pUnit->isMoving();

    default:
        _ASSERTE(!"Invalid Entity Attribute");
    }

    return 0;
}
//----------------------------------------------------------------------------------------------
ObjectStateType StarCraftEntity::FetchState() const
{
    bool isIdle = m_pUnit->isIdle();
    bool isCompleted = m_pUnit->isCompleted();
    bool isBeingConstructed = m_pUnit->isBeingConstructed();
    bool isConstructing = m_pUnit->isConstructing();
    bool isMoving = m_pUnit->isMoving();
    bool isGatheringGas = m_pUnit->isGatheringGas();
    bool isGatheringMinerals = m_pUnit->isGatheringMinerals();
    bool isTraining = m_pUnit->isTraining();
    bool isAttacking = m_pUnit->isAttacking();
    bool isUnderAttack = m_pUnit->isUnderAttack();

    if (isIdle && isCompleted)
        return OBJSTATE_Idle;
    else if (isBeingConstructed || (isIdle && !isCompleted))
        return OBJSTATE_BeingConstructed;
    else if (isGatheringGas || isGatheringMinerals)
        return OBJSTATE_Gathering;
    else if (isConstructing)
        return OBJSTATE_Constructing;
    else if (isMoving)
        return OBJSTATE_Moving;
    else if (isTraining)
        return OBJSTATE_Training;
    else if (isAttacking)
        return OBJSTATE_Attacking;
    else if (isUnderAttack)
        return OBJSTATE_UnderAttack;
    else
        return OBJSTATE_END;
}
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::Research(ResearchType p_researchId)
{
    bool bOk;

    // Is tech
    if ((int)p_researchId >= ((int)(RESEARCH_START +  TechIdOffset)))
    {
        TID gameTypeID = g_Database.TechMapping.GetBySecond(p_researchId);

        bOk = m_pUnit->research(TechType(gameTypeID));
    }
    // Is upgrade
    else
    {
        TID gameTypeID = g_Database.UpgradeMapping.GetBySecond(p_researchId);
        bOk = m_pUnit->upgrade(UpgradeType(gameTypeID));
    }

    return bOk;
}
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::Build(EntityClassType p_buildingClassId, Vector2 p_position) 
{
    TilePosition   pos(TilePositionFromUnitPosition(p_position.X), TilePositionFromUnitPosition(p_position.Y));
    UnitType type;
    TID             gameTypeId;
    string         typeName;

    gameTypeId = g_Database.EntityMapping.GetBySecond(p_buildingClassId);
    typeName = g_Database.EntityIdentMapping.GetByFirst(gameTypeId);

    type = UnitType::getType(typeName);
    type = BWAPI::UnitType::getType(typeName);

    return m_pUnit->build(type, pos);
};
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::AttackGround(Vector2 p_position)
{
    Position pos(p_position.X, p_position.Y);
    return m_pUnit->attack(pos);
};
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::AttackEntity(TID p_targetEntityObjectId)
{
    Unit attacker = m_pUnit;
    Unit target;

    target = Broodwar->getUnit(p_targetEntityObjectId);

    if (!target)
        throw ItemNotFoundException(XcptHere);

    return attacker->attack(target->getPosition());
};
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::Train(EntityClassType p_entityClassId)
{
    Unit            building = m_pUnit;
    TID             unitTypeId;
    string          typeName;
    UnitType        type;

    unitTypeId = g_Database.EntityMapping.GetBySecond(p_entityClassId);
    typeName = g_Database.EntityIdentMapping.GetByFirst(unitTypeId);

    type = BWAPI::UnitType::getType(typeName);

    return building->train(type);

};
//----------------------------------------------------------------------------------------------
bool StarCraftModel::StarCraftEntity::IsTraining(TID p_traineeId) const
{
    Unit traineeObj = Broodwar->getUnit(p_traineeId);

    if (nullptr == traineeObj)
        throw ItemNotFoundException(XcptHere);

    return MathHelper::RectangleMembership(
        m_pUnit->getLeft(),
        m_pUnit->getTop(),
        m_pUnit->getRight() - m_pUnit->getLeft(),
        m_pUnit->getBottom() - m_pUnit->getTop(),
        traineeObj->getPosition().x,
        traineeObj->getPosition().y);
}
//----------------------------------------------------------------------------------------------
string StarCraftModel::StarCraftEntity::ToString() const
{
    std::string asSharedResource = SharedResource::ToString();

    std::string description = m_pUnit->getType().getName();
    description += "(";
    description += to_string((long long)m_pUnit->getID());
    description += ",";
    description += asSharedResource;
    description += ")";

    return description;
}
//----------------------------------------------------------------------------------------------
Vector2 StarCraftEntity::GetPosition() const
{
    return Vector2(m_pUnit->getPosition().x, m_pUnit->getPosition().y);
}
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::Move(Vector2 p_position)
{
    Position pos(p_position.X, p_position.Y);
    return m_pUnit->move(pos);
}
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::IsNull()
{
    return m_pUnit == nullptr;
}
//----------------------------------------------------------------------------------------------
IClonable* StarCraftEntity::Clone()
{
    StarCraftEntity* pClone = new StarCraftEntity();
    Copy(pClone);

    return pClone;
}
//----------------------------------------------------------------------------------------------
void StarCraftEntity::Copy(IClonable* pDest)
{
    StarCraftEntity* pConDest = dynamic_cast<StarCraftEntity*>(pDest);
    _ASSERTE(pConDest);

    GameEntity::Copy(pDest);
    pConDest->m_pUnit = m_pUnit;
}
