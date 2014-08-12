#include "GameEntity.h"
#include "RtsGame.h"
#include "GameType.h"
#include "EntityController.h"
#include "WorldMap.h"
#include "GamePlayer.h"
#include "MathHelper.h"
#include "GameResearch.h"

using namespace IStrategizer;
using namespace std;

GameEntity::GameEntity() :
m_id(INVALID_TID),
m_cacheFrame(INT_MIN),
m_pController(nullptr),
m_isOnline(true),
m_pUnitType(nullptr),
m_typeId(ECLASS_END),
m_ownerId(PLAYER_END)
{}

GameEntity::GameEntity(TID unitId) :
m_id(unitId),
m_cacheFrame(INT_MIN),
m_pController(nullptr),
m_isOnline(true)
{
    m_pUnitType = g_GameImpl->UnitGetType(unitId);

    m_typeId = m_pUnitType->EngineId();
    m_cachedAttr[OP_TypeId] = m_typeId;
    m_pType = g_Game->GetEntityType(m_typeId);

    m_ownerId = g_GameImpl->PlayerGetType(g_GameImpl->UnitPlayer(unitId));
    m_cachedAttr[OP_OwnerId] = m_ownerId;
}
//////////////////////////////////////////////////////////////////////////
const GameType* GameEntity::Type() const
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    return m_pType;
}
//////////////////////////////////////////////////////////////////////////
unsigned GameEntity::Distance(_In_ const GameEntity *pOther) const
{
    int dX = P(OP_PosCenterX) - pOther->P(OP_PosCenterX);
    int dY = P(OP_PosCenterY) - pOther->P(OP_PosCenterY);
    return (unsigned)sqrt((float)(unsigned)((dX * dX) + (dY * dY)));
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::CacheAttributes()
{
    if ((int)g_Game->GameFrame() > m_cacheFrame)
    {
        LogDebugInfo("Caching %s attributes", ToString().c_str());

        m_cacheFrame = g_Game->GameFrame();

        m_cachedAttr.clear();
        for (int attr = START(EntityObjectProperty);
            attr != END(EntityObjectProperty); ++attr)
        {
            m_cachedAttr[EntityObjectProperty(attr)] = P(EntityObjectProperty(attr));
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::DebugDrawTarget()
{
    auto targetPos = TargetPosition();

    if (targetPos.IsInf())
        return;

    auto selfPos = Position();

    if (selfPos == targetPos)
        return;

    g_Game->DebugDrawMapLine(selfPos, targetPos, GCLR_Red);
    g_Game->DebugDrawMapCircle(targetPos, 3, GCLR_Red, true);
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::DebugDrawRange()
{
    g_Game->DebugDrawMapCircle(Position(), Type()->P(TP_GroundRange), GCLR_Yellow);
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::DebugDrawLineOfSight()
{
    g_Game->DebugDrawMapCircle(Position(), Type()->P(TP_LineOfSight), GCLR_White);
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::DebugDrawHealthBar()
{
    int barHeight = 1;

    Vector2 barTopLeft(P(OP_Left), P(OP_Bottom));
    Vector2 barBottomRight(P(OP_Right), barTopLeft.Y + barHeight);

    g_Game->DebugDrawMapRectangle(barTopLeft, barBottomRight, GCLR_Red, true);

    int fillHeight = 3;
    float healthPrct = (float)P(OP_Health) / (float)Type()->P(TP_MaxHp);

    int barWidth = barBottomRight.X - barTopLeft.X;
    Vector2 fillTopLeft(barTopLeft.X, barBottomRight.Y);
    Vector2 fillBottomRight(barTopLeft.X + (int)(healthPrct * (float)barWidth), fillTopLeft.Y + fillHeight);

    g_Game->DebugDrawMapRectangle(fillTopLeft, fillBottomRight, GCLR_Red, true);
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::DebugDraw()
{
    DebugDrawHealthBar();
    DebugDrawTarget();

    if (m_pController)
        m_pController->DebugDraw();
}
//////////////////////////////////////////////////////////////////////////
int GameEntity::P(EntityObjectProperty attrId) const
{
    // Attributes are accessible from game directly if the entity is online in 2 cases only
    // 1- Unit is visible to player
    // 2- Unit belongs to neutral and game frame is 0
    // 3- Unit is about to hide and this frame is marked as cache frame
    bool isVisible = g_GameImpl->UnitIsVisible(m_id);
    bool isNeutralAtFrame0 = g_GameImpl->PlayerIsNeutral(g_GameImpl->UnitPlayer(m_id)) && g_Game->GameFrame() == 0;
    bool isCacheFrame = (int)g_Game->GameFrame() == m_cacheFrame;

    if (m_isOnline &&
        (isVisible || isNeutralAtFrame0 || isCacheFrame))
    {
        // Positions are measured in pixels and are the highest resolution
        // Walk Tiles - each walk tile is an 8x8 square of pixels. These are called walk tiles because walkability data is available at this resolution.
        // Build Tiles - each build tile is a 4x4 square of walk tiles, or a 32x32 square of pixels.
        // These are called build tiles because buildability data is available at this resolution, and correspond to the tiles seen in game.
        // For example, a Command Center occupies an area of 4x3 build tiles. Build tiles can be specified in TilePosition objects.
        // We will use walk tiles as a measure of positions units across IStrategizer
        // See: http://code.google.com/p/bwapi/wiki/Misc

        switch (attrId)
        {
        case OP_Health:
            return g_GameImpl->UnitHitpoints(m_id);

        case OP_Left:
            if (m_pUnitType->IsBuilding())
                return UnitCoordFromTileCoord(g_GameImpl->UnitTilePosition(m_id).X);
            else
                return g_GameImpl->UnitTopLeft(m_id).X;

        case OP_Top:
            if (m_pUnitType->IsBuilding())
                return UnitCoordFromTileCoord(g_GameImpl->UnitTilePosition(m_id).Y);
            else
                return g_GameImpl->UnitTopLeft(m_id).Y;

        case OP_Right:
            if (m_pUnitType->IsBuilding())
                return UnitCoordFromTileCoord(g_GameImpl->UnitTilePosition(m_id).X + m_pUnitType->TileWidth());
            else
                return g_GameImpl->UnitBottomRight(m_id).X;

        case OP_Bottom:
            if (m_pUnitType->IsBuilding())
                return UnitCoordFromTileCoord(g_GameImpl->UnitTilePosition(m_id).Y + m_pUnitType->TileHeight());
            else
                return g_GameImpl->UnitBottomRight(m_id).Y;

        case OP_State:
            return FetchState();

        case OP_OwnerId:
            return m_ownerId;

        case OP_TypeId:
            return m_typeId;

        case OP_PosCenterX:
            return g_GameImpl->UnitPosition(m_id).X;

        case OP_PosCenterY:
            return g_GameImpl->UnitPosition(m_id).Y;

        case OP_IsGatheringSecondaryResource:
            return g_GameImpl->UnitIsGatheringGas(m_id);

        case OP_IsGatheringPrimaryResource:
            return g_GameImpl->UnitIsGatheringMinerals(m_id);

        case OP_IsBeingGathered:
            return g_GameImpl->UnitIsBeingGathered(m_id);

        case OP_OrderTargetId:
            return TargetId();

        case OP_IsBeingHit:
            return g_GameImpl->UnitIsUnderAttack(m_id);

        case OP_IsMoving:
            return g_GameImpl->UnitIsMoving(m_id);

        case OP_IsAttacking:
            return g_GameImpl->UnitIsAttacking(m_id) || g_GameImpl->UnitIsInAttackFrame(m_id);

        case OP_IsVisible:
            return g_GameImpl->UnitIsVisible(m_id) && g_GameImpl->UnitIsDetected(m_id);

        case OP_IsRepairing:
            return g_GameImpl->UnitIsRepairing(m_id);

        case OP_BuildEntityId:
            return g_GameImpl->UnitBuildUnit(m_id);

        case OP_IsTargetable:
            return g_GameImpl->UnitIsVisible(m_id) && g_GameImpl->UnitIsDetected(m_id) && g_GameImpl->UnitIsTargetable(m_id);

        case OP_IsTraining:
            return g_GameImpl->UnitIsTraining(m_id);

        default:
            DEBUG_THROW(InvalidParameterException(XcptHere));
        }
    }
    else
    {
        return m_cachedAttr.at(attrId);
    }
}
//////////////////////////////////////////////////////////////////////////
ObjectStateType GameEntity::FetchState() const
{
    bool isIdle = g_GameImpl->UnitIsIdle(m_id);
    bool isCompleted = g_GameImpl->UnitIsCompleted(m_id);
    bool isBeingConstructed = g_GameImpl->UnitIsBeingConstructed(m_id);
    bool isConstructing = g_GameImpl->UnitIsConstructing(m_id);
    bool isMoving = g_GameImpl->UnitIsMoving(m_id);
    bool isGatheringGas = g_GameImpl->UnitIsGatheringGas(m_id);
    bool isGatheringMinerals = g_GameImpl->UnitIsGatheringMinerals(m_id);
    bool isTraining = g_GameImpl->UnitIsTraining(m_id);
    bool isAttacking = g_GameImpl->UnitIsAttacking(m_id);
    bool isUnderAttack = g_GameImpl->UnitIsUnderAttack(m_id);

    if (isIdle && isCompleted && g_GameImpl->UnitIsInterruptible(m_id))
        return OBJSTATE_Idle;
    else if (isBeingConstructed || (isIdle && !isCompleted))
        return OBJSTATE_BeingConstructed;
    else if (isGatheringMinerals)
        return OBJSTATE_GatheringPrimary;
    else if (isGatheringGas)
        return OBJSTATE_GatheringSecondary;
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
//////////////////////////////////////////////////////////////////////////
bool GameEntity::IsTraining(TID p_traineeId) const
{
    GameEntity* pTrainee = g_Game->GetPlayer((PlayerType)P(OP_OwnerId))->GetEntity(p_traineeId);

    if (nullptr == pTrainee)
        DEBUG_THROW(ItemNotFoundException(XcptHere));

    return pTrainee->P(OP_State) == OBJSTATE_BeingConstructed &&
        MathHelper::RectangleMembership(
        P(OP_Left),
        P(OP_Top),
        P(OP_Right) - P(OP_Left),
        P(OP_Bottom) - P(OP_Top),
        pTrainee->P(OP_PosCenterX),
        pTrainee->P(OP_PosCenterY));
}
//////////////////////////////////////////////////////////////////////////
string GameEntity::ToString(bool minimal) const
{
    char str[512];
    std::string description = m_pUnitType->ToString();

    if (minimal)
        sprintf_s(str, "%s[%d]", description.c_str(), m_id);
    else
    {
        std::string asResource = SharedResource::ToString();
        sprintf_s(str, "%s[%d](%s, State=%s)", description.c_str(), m_id, asResource.c_str(), Enums[P(OP_State)]);
    }
    return str;
}
//////////////////////////////////////////////////////////////////////////
Vector2 GameEntity::Position() const
{
    return Vector2(P(OP_PosCenterX), P(OP_PosCenterY));
}
//////////////////////////////////////////////////////////////////////////
bool GameEntity::CanGather(TID resourceObjectId) const
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    return g_GameImpl->UnitCanGather(m_id, resourceObjectId);
}
//////////////////////////////////////////////////////////////////////////
bool GameEntity::Build(EntityClassType typeId, Vector2 pos)
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    auto pUnitType = g_GameImpl->GetUnitTypeByEngineId(typeId);
    LogInfo("%s -> Build(%s@<%d,%d>)", ToString().c_str(), pUnitType->ToString(), pos.X, pos.Y);

    bool bOk = false;

    if (pUnitType->IsAddon())
        bOk = g_GameImpl->UnitBuildAddon(m_id, pUnitType);
    else
    {

        Vector2 tilePos = TilePositionFromUnitPosition(pos);
        bOk = g_GameImpl->UnitBuild(m_id, pUnitType, tilePos);

        g_GameImpl->DebugDrawUnitBuildBox(pUnitType, tilePos, (bOk ? GCLR_Blue : GCLR_Purple));
    }

    DebugDrawMapLastGameError();

    return bOk;
};
//////////////////////////////////////////////////////////////////////////
bool GameEntity::Repair(_In_ TID targetId)
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    LogInfo("%s -> Repair(%s)", ToString().c_str(), g_Game->GetEntity(targetId)->ToString().c_str());

    if (!g_GameImpl->UnitExists(m_id))
    {
        LogWarning("Entity %s command drop, same target %d", ToString().c_str(), targetId);
        return false;
    }

    _ASSERTE(g_GameImpl->UnitIsDetected(targetId));

    if (g_GameImpl->UnitRepair(m_id, targetId))
    {
        return true;
    }
    else
    {
        DebugDrawMapLastGameError();
        return false;
    }
}
//////////////////////////////////////////////////////////////////////////
bool GameEntity::Research(ResearchType researchId)
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    LogInfo("%s -> Research(%s)", ToString().c_str(), g_Game->GetResearch(researchId)->ToString().c_str());

    bool bOk = false;

    // Is tech
    if (g_GameImpl->GetTechTypeByEngineId(researchId) != nullptr)
    {
        bOk = g_GameImpl->UnitResearch(m_id, g_GameImpl->GetTechTypeByEngineId(researchId));
    }
    // Is upgrade
    else
    {
        bOk = g_GameImpl->UnitUpgrade(m_id, g_GameImpl->GetUpgradeTypeByEngineId(researchId));
    }

    DebugDrawMapLastGameError();

    return bOk;
}
//////////////////////////////////////////////////////////////////////////
bool GameEntity::AttackGround(Vector2 p_position)
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    if (g_GameImpl->UnitAttackMove(m_id, p_position))
        return true;
    else
    {
        DebugDrawMapLastGameError();
        return false;
    }
};
//////////////////////////////////////////////////////////////////////////
bool GameEntity::Stop()
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    LogInfo("%s -> Stop", ToString().c_str());

    if (g_GameImpl->UnitStop(m_id))
    {
        return true;
    }
    else
    {
        DebugDrawMapLastGameError();
        return false;
    }
};
//////////////////////////////////////////////////////////////////////////
bool GameEntity::AttackEntity(TID targetId)
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    LogInfo("%s -> Attack(%s)", ToString().c_str(), g_Game->GetEntity(targetId)->ToString().c_str());

    if (!g_GameImpl->UnitExists(m_id))
    {
        LogWarning("Entity %s command drop, same target %d", ToString().c_str(), targetId);
        return false;
    }

    _ASSERTE(g_GameImpl->UnitIsDetected(targetId));

    if (g_GameImpl->UnitAttack(m_id, targetId))
    {
        return true;
    }
    else
    {
        DebugDrawMapLastGameError();
        return false;
    }
};
//////////////////////////////////////////////////////////////////////////
bool GameEntity::Train(EntityClassType p_entityClassId)
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    auto pUnitType = g_GameImpl->GetUnitTypeByEngineId(p_entityClassId);
    LogInfo("%s -> Train(Trainee=%s)", ToString().c_str(), pUnitType->ToString());

    bool bOk = g_GameImpl->UnitTrain(m_id, pUnitType);

    DebugDrawMapLastGameError();

    return bOk;
};
//////////////////////////////////////////////////////////////////////////
bool GameEntity::Move(Vector2 targetPos)
{
    _ASSERTE(!targetPos.IsInf());
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    LogInfo("%s -> Move(%s)", ToString().c_str(), targetPos.ToString().c_str());

    //_ASSERTE(Broodwar->isWalkable(WalkPositionFromUnitPosition(pos.x), WalkPositionFromUnitPosition(pos.y)));
    if (g_GameImpl->UnitMove(m_id, targetPos))
        return true;
    else
    {
        DebugDrawMapLastGameError();
        return false;
    }
}
//////////////////////////////////////////////////////////////////////////
bool GameEntity::GatherResourceEntity(TID resourceId)
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    auto pResourceType = g_GameImpl->UnitGetType(resourceId);

    LogInfo("%s -> GatherResource(Resource=%s)", ToString().c_str(), pResourceType->ToString());

    if (g_GameImpl->UnitGather(m_id, resourceId))
        return true;
    else
    {
        DebugDrawMapLastGameError();
        return false;
    }
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::SetOffline(RtsGame* pBelongingGame)
{
    CacheAttributes();
    m_isOnline = false;
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::CancelConstruction()
{
    if (g_GameImpl->UnitCancelConstruction(m_id))
    {
        LogInfo("%s canceled building construction", ToString().c_str());
    }
}
//////////////////////////////////////////////////////////////////////////
bool GameEntity::CanTrain(EntityClassType type)
{
    if (!m_isOnline)
        DEBUG_THROW(InvalidOperationException(XcptHere));

    return g_GameImpl->UnitCanTrain(m_id, g_GameImpl->GetUnitTypeByEngineId(type));
}
//////////////////////////////////////////////////////////////////////////
bool GameEntity::Exists() const
{
    return g_GameImpl->UnitExists(m_id);
}
//////////////////////////////////////////////////////////////////////////
TID GameEntity::TargetId() const
{
    TID target = INVALID_TID;

    // Exhaustive ordered search in all possible positions a unit can head to
    target = g_GameImpl->UnitTarget(m_id);

    if (target == INVALID_TID)
        target = g_GameImpl->UnitOrderTarget(m_id);

    return target;
}
//////////////////////////////////////////////////////////////////////////
Vector2 GameEntity::TargetPosition() const
{
    Vector2 pos = Vector2::Inf();
    TID target = INVALID_TID;


    // Exhaustive ordered search in all possible positions a unit can head to
    target = g_GameImpl->UnitTarget(m_id);

    if (target != INVALID_TID)
    {
        pos = g_GameImpl->UnitPosition(target);
    }
    else
    {
        target = g_GameImpl->UnitOrderTarget(m_id);

        if (target != INVALID_TID)
        {
            pos = g_GameImpl->UnitPosition(target);
        }
        else
        {
            pos = g_GameImpl->UnitTargetPosition(m_id);

            if (pos.IsInf())
            {
                pos = g_GameImpl->UnitOrderTargetPosition(m_id);
            }
        }
    }

    return pos;
}
//////////////////////////////////////////////////////////////////////////
bool GameEntity::CanAttack(_In_ TID targetId) const
{
    return g_GameImpl->UnitExists(targetId) &&
        g_GameImpl->UnitCanAttackUnit(m_id, targetId) &&
        g_GameImpl->UnitIsVisible(m_id) &&
        g_GameImpl->UnitIsDetected(m_id);
}
//////////////////////////////////////////////////////////////////////////
void GameEntity::DebugDrawMapLastGameError()
{
    g_GameImpl->DebugDrawMapLastGameError(m_id);

    char buff[128];
    g_GameImpl->LastError(buff, 128);
    LogInfo("Game Last Error: %s", buff);
}
//////////////////////////////////////////////////////////////////////////
bool GameEntity::CanRepair(TID entityId) const
{
    return g_GameImpl->UnitCanRepair(m_id, entityId);
}
//////////////////////////////////////////////////////////////////////////
bool GameEntity::CanReach(_In_ Vector2 dest) const
{
    auto selfPos = Position();
    return !g_GameImpl->UnitIsStuck(m_id) &&
        g_GameImpl->MapHasPath(selfPos, dest);
}
//////////////////////////////////////////////////////////////////////////
int GameEntity::LastCommandFrame() const
{
    return g_GameImpl->UnitLastCommandFrame(m_id);
}
//////////////////////////////////////////////////////////////////////////
bool GameEntity::CanBuild(_In_ EntityClassType typeId) const
{
    auto pUnitType = g_GameImpl->GetUnitTypeByEngineId(typeId);

    if (pUnitType->IsAddon())
        return g_GameImpl->UnitCanBuildAddOn(m_id, pUnitType);
    else
        return g_GameImpl->UnitCanBuild(m_id, pUnitType);
}
//////////////////////////////////////////////////////////////////////////
float GameEntity::HitpointsPercentage() const
{
    return (float)P(OP_Health) / (float)m_pType->P(TP_MaxHp);
}
//////////////////////////////////////////////////////////////////////////
bool GameEntity::IsTargetInWeaponRage(_In_ TID entityId) const
{
    return g_GameImpl->UnitTargetInWeaponRage(m_id, entityId);
}