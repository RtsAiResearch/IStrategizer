#include "EntityController.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameEntity.h"
#include "GameType.h"
#include "IMSystemManager.h"
#include "GroundControlIM.h"
#include "EntityFSM.h"
#include "ArmyController.h"

using namespace IStrategizer;

Vector2 EntityController::TargetPosition() const 
{ 
    // If my target position is not set, always return the army target position
    // If my target position is set, then return my override position
    // otherwise if I am part of an army, then return controller position, otherwise
    // return my position
    if (m_singleTargetPos.IsInf())
        return (m_pController != nullptr ? m_pController->TargetPosition() : m_singleTargetPos);
    else
        return m_singleTargetPos;
}
//////////////////////////////////////////////////////////////////////////
void EntityController::ControlEntity(_In_ TID entityId)
{
    if (m_entityId != INVALID_TID)
        ReleaseEntity();

    m_entityId = entityId;

    auto pScout = g_Game->Self()->GetEntity(m_entityId);
    _ASSERTE(pScout);
    pScout->Lock(this);
}
//////////////////////////////////////////////////////////////////////////
void EntityController::ReleaseEntity()
{
    if (m_entityId != INVALID_TID)
    {
        auto pScout = g_Game->Self()->GetEntity(m_entityId);

        if (pScout && pScout->IsLocked())
            pScout->Unlock(this);

        m_entityId = INVALID_TID;

        while (!m_pLogicMemory.empty())
            m_pLogicMemory.pop();

        m_pLogicMemory.push(StackFSMPtr(new IdleEntityFSM(this)));
    }
}
//////////////////////////////////////////////////////////////////////////
void EntityController::Update()
{
    if (m_entityId == INVALID_TID)
        return;

    m_pLogicMemory.top()->Update();

    if (m_pLogicMemory.top()->TypeId() == FleeEntityState::TypeID &&
        m_pController != nullptr)
    {
        m_pController->OnControlledEntityFlee(this);
    }
}
//////////////////////////////////////////////////////////////////////////
GameEntity* EntityController::Entity()
{
    return g_Game->Self()->GetEntity(m_entityId);
}
//////////////////////////////////////////////////////////////////////////
const GameEntity* EntityController::Entity() const
{
    return g_Game->Self()->GetEntity(m_entityId);
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::EntityExists() const
{
    return EntityExists(m_entityId);
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::IsOnCriticalHP(_In_ const GameEntity* pEntity)
{
    auto currentHp = pEntity->P(OP_Health);
    auto maxHp = pEntity->Type()->P(TP_MaxHp);
    auto criticalHp = int(0.15 * (float)maxHp);

    return currentHp <= criticalHp;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::IsOnCriticalHP()
{
    if (!IsControllingEntity() ||
        !EntityExists())
        return false;

    return IsOnCriticalHP(Entity());
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::IsBeingHit()
{
    if (!IsControllingEntity() ||
        !EntityExists())
        return false;

    return Entity()->P(OP_IsBeingHit) > 0;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::ArrivedAtTarget(_In_ Vector2 pos)
{
    if (!IsControllingEntity() ||
        !EntityExists() ||
        pos.IsInf())
        return false;

    auto distToTarget = pos.Distance(Entity()->GetPosition());

    return distToTarget <= PositionArriveRadius;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::ThreatAtTarget(_In_ Vector2 pos)
{
    if (!IsControllingEntity() ||
        !EntityExists() ||
        pos.IsInf())
        return false;

    auto pGrnCtrlIM = (GroundControlIM*)g_IMSysMgr.GetIM(IM_GroundControl);
    return pGrnCtrlIM->GetCellInfluenceFromWorldPosition(pos) < 0;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::IsTargetInSight(_In_ Vector2 pos)
{
    if (!IsControllingEntity() ||
        !EntityExists() ||
        pos.IsInf())
        return false;

    int los = Entity()->Type()->P(TP_LineOfSight);
    Circle2 sight(Entity()->GetPosition(), los);

    return sight.IsInside(pos);
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::IsTargetInSight(_In_ TID entityId)
{
    if (!IsControllingEntity() ||
        !EntityExists())
        return false;

    auto pEntity = g_Game->GetEntity(entityId);
    return IsTargetInSight(pEntity->GetPosition());
}
//////////////////////////////////////////////////////////////////////////
TID EntityController::GetClosestEnemyEntityInSight()
{
    if (!IsControllingEntity() ||
        !EntityExists())
        return false;

    EntityList enemies;

    int closestDist = INT_MAX;
    TID closestId = INVALID_TID;
    Vector2 selfPos = Entity()->GetPosition();
    Vector2 otherPos = Vector2::Inf();
    int los = Entity()->Type()->P(TP_LineOfSight);

    // For now, just select the closest entity in sight
    for (auto& entityR : g_Game->Enemy()->Entities())
    {
        if (!Entity()->CanAttack(entityR.first))
            continue;

        otherPos = entityR.second->GetPosition();
        int dist = selfPos.Distance(otherPos);

        if (dist < los && dist < closestDist)
        {
            closestId = entityR.first;
            closestDist = dist;
        }
    }

    return closestId;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::IsAnyEnemyTargetInSight()
{
    if (!IsControllingEntity() ||
        !EntityExists())
        return false;

    EntityList enemies;

    int los = Entity()->Type()->P(TP_LineOfSight);
    Circle2 sight(Entity()->GetPosition(), los);

    for (auto& entityR : g_Game->Enemy()->Entities())
    {
        if (sight.IsInside(entityR.second->GetPosition()) &&
            Entity()->CanAttack(entityR.first))
            return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::EntityExists(_In_ TID entityId) const
{
    auto pEntity = g_Game->GetEntity(entityId);
    return pEntity != nullptr && pEntity->Exists();
}