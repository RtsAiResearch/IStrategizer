#ifndef STARCRAFTENTITY_H
#include "StarCraftEntity.h"
#endif
#ifndef METADATA_H
#include "MetaData.h"
#endif
#ifndef DEFINITIONCROSSMAPPING_H
#include "DefinitionCrossMapping.h"
#endif
#include "IStrategizerException.h"
#include "MathHelper.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "ObjectFactory.h"

using namespace IStrategizer;
using namespace BWAPI;
using namespace std;

DECL_SERIALIZABLE(StarCraftEntity);

StarCraftEntity::StarCraftEntity(Unit p_unit) :
    GameEntity(p_unit->getID()),
    m_pUnit(p_unit),
    m_isOnline(true)
{
}
//----------------------------------------------------------------------------------------------
int StarCraftEntity::Attr(EntityObjectAttribute attrId) const
{
    if (m_isOnline)
    {
        // Positions are measured in pixels and are the highest resolution
        // Walk Tiles - each walk tile is an 8x8 square of pixels. These are called walk tiles because walkability data is available at this resolution.
        // Build Tiles - each build tile is a 4x4 square of walk tiles, or a 32x32 square of pixels.
        // These are called build tiles because buildability data is available at this resolution, and correspond to the tiles seen in game.
        // For example, a Command Center occupies an area of 4x3 build tiles. Build tiles can be specified in TilePosition objects.
        // We will use walk tiles as a measure of positions units across IStrategizer
        // See: http://code.google.com/p/bwapi/wiki/Misc

        switch(attrId)
        {
        case EOATTR_Health:
            return m_pUnit->getHitPoints();

        case EOATTR_Left:
            if (m_pUnit->getType().isBuilding())
                return UnitPositionFromTilePosition(m_pUnit->getTilePosition().x);
            else
                return m_pUnit->getLeft();

        case EOATTR_Top:
            if (m_pUnit->getType().isBuilding())
                return UnitPositionFromTilePosition(m_pUnit->getTilePosition().y);
            else
                return m_pUnit->getTop();

        case EOATTR_Right:
            if (m_pUnit->getType().isBuilding())
                return UnitPositionFromTilePosition(m_pUnit->getTilePosition().x + m_pUnit->getType().tileWidth());
            else
                return m_pUnit->getRight();

        case EOATTR_Bottom:
            if (m_pUnit->getType().isBuilding())
                return UnitPositionFromTilePosition(m_pUnit->getTilePosition().y + m_pUnit->getType().tileHeight());
            else
                return m_pUnit->getBottom();

        case EOATTR_State:
            return FetchState();

        case EOATTR_OwnerId:
            return g_Database.PlayerMapping.GetByFirst(m_pUnit->getPlayer()->getID());

        case EOATTR_Type:
            return g_Database.EntityMapping.GetByFirst(m_pUnit->getType().getID());

        case EOATTR_PosCenterX:
            return m_pUnit->getPosition().x;

        case EOATTR_PosCenterY:
            return m_pUnit->getPosition().y;

        case EOATTR_IsMoving:
            return m_pUnit->isMoving();

        case EOATTR_IsGatheringPrimaryResource:
            return m_pUnit->isGatheringMinerals() || m_pUnit->isCarryingMinerals();

        case EOATTR_IsGatheringSecondaryResource:
            return m_pUnit->isGatheringGas() || m_pUnit->isCarryingGas();

        default:
            DEBUG_THROW(InvalidParameterException(XcptHere));
        }
    }
    else
    {
        _ASSERTE(m_cachedAttr.Contains(attrId));
        return m_cachedAttr.at(attrId);
    }
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
bool StarCraftEntity::IsTraining(TID p_traineeId) const
{
    GameEntity* pTrainee = g_Game->GetPlayer((PlayerType)Attr(EOATTR_OwnerId))->GetEntity(p_traineeId);

    if (nullptr == pTrainee)
        DEBUG_THROW(ItemNotFoundException(XcptHere));

    return MathHelper::RectangleMembership(
        Attr(EOATTR_Left),
        Attr(EOATTR_Top),
        Attr(EOATTR_Right) - Attr(EOATTR_Left),
        Attr(EOATTR_Bottom) - Attr(EOATTR_Top),
        pTrainee->Attr(EOATTR_PosCenterX),
        pTrainee->Attr(EOATTR_PosCenterY));
}
//----------------------------------------------------------------------------------------------
string StarCraftEntity::ToString() const
{
    std::string asSharedResource = SharedResource::ToString();

    TID gameTypeId = g_Database.EntityMapping.GetBySecond((EntityClassType)Attr(EOATTR_Type));
    std::string description = g_Database.EntityIdentMapping.GetByFirst(gameTypeId);
    description += "(";
    description += to_string((long long)m_id);
    description += ",";
    description += asSharedResource;
    description += ")";

    return description;
}
//----------------------------------------------------------------------------------------------
Vector2 StarCraftEntity::GetPosition() const
{
    return Vector2(Attr(EOATTR_PosCenterX), Attr(EOATTR_PosCenterY));
}
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::CanGather(TID resourceObjectId) const
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    return m_pUnit->canGather(Broodwar->getUnit(resourceObjectId));
}
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::Build(EntityClassType p_buildingClassId, Vector2 p_position) 
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    TilePosition pos(TilePositionFromUnitPosition(p_position.X), TilePositionFromUnitPosition(p_position.Y));
    UnitType type;
    TID gameTypeId;
    string typeName;

    gameTypeId = g_Database.EntityMapping.GetBySecond(p_buildingClassId);
    typeName = g_Database.EntityIdentMapping.GetByFirst(gameTypeId);

    type = UnitType::getType(typeName);
    type = BWAPI::UnitType::getType(typeName);

    if (type.isAddon())
    {
        _ASSERTE(m_pUnit->canBuildAddon(type));
        return m_pUnit->buildAddon(type);
    }
    else
    {
        // _ASSERTE(Broodwar->canBuildHere(pos, type));
        return m_pUnit->build(type, pos);
    }
};
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::Research(ResearchType p_researchId)
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

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
bool StarCraftEntity::AttackGround(Vector2 p_position)
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    Position pos(p_position.X, p_position.Y);
    return m_pUnit->attack(pos);
};
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::AttackEntity(TID p_targetEntityObjectId)
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    Unit attacker = m_pUnit;
    Unit target;

    target = Broodwar->getUnit(p_targetEntityObjectId);

    if (!target)
        DEBUG_THROW(ItemNotFoundException(XcptHere));

    return attacker->attack(target->getPosition());
};
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::Train(EntityClassType p_entityClassId)
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    Unit building = m_pUnit;
    TID unitTypeId = g_Database.EntityMapping.GetBySecond(p_entityClassId);
    string typeName = g_Database.EntityIdentMapping.GetByFirst(unitTypeId);
    UnitType type = BWAPI::UnitType::getType(typeName);

    _ASSERTE(building->canTrain(type));
    bool success = building->train(type);

    return success;
};
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::Move(Vector2 p_position)
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    Position pos(p_position.X, p_position.Y);
    return m_pUnit->move(pos);
}
//----------------------------------------------------------------------------------------------
bool StarCraftEntity::GatherResourceEntity(TID p_resourceEntityObjectId)
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    Unit gatherer = m_pUnit;
    Unit resource;

    resource = Broodwar->getUnit(p_resourceEntityObjectId);
    _ASSERTE(resource);

    _ASSERTE(gatherer->canGather(resource));
    return gatherer->gather(resource);
}

void StarCraftEntity::SetOffline(RtsGame* pBelongingGame)
{
    m_cachedAttr.clear();
    for (int attr = START(EntityObjectAttribute);
        attr != END(EntityObjectAttribute); ++attr)
    {
        m_cachedAttr[EntityObjectAttribute(attr)] = Attr(EntityObjectAttribute(attr));
    }

    m_isOnline = false;
}