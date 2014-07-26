#include "EntityController.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameEntity.h"
#include "GameType.h"
#include "IMSystemManager.h"
#include "GroundControlIM.h"
#include "EntityFSM.h"

using namespace IStrategizer;

void EntityController::ControlEntity(_In_ TID entityId)
{
    if (m_entityId != INVALID_TID)
        ReleaseEntity();

    m_entityId = entityId;
    m_pLogic = StackFSMPtr(new IdleEntityFSM(this));

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
    }
}
//////////////////////////////////////////////////////////////////////////
void EntityController::Update()
{
    if (m_entityId == INVALID_TID)
        return;

    m_pLogic->Update();
}
//////////////////////////////////////////////////////////////////////////
GameEntity* EntityController::Entity()
{
    return g_Game->Self()->GetEntity(m_entityId);
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::EntityExists() const
{
    return EntityExists(m_entityId);
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::IsOnCriticalHP()
{
    if (!IsControllingEntity() ||
        !EntityExists())
        return false;

    auto pEntity = g_Game->Self()->GetEntity(m_entityId);
    auto currentHp = pEntity->Attr(EOATTR_Health);
    auto maxHp = pEntity->Type()->Attr(ECATTR_MaxHp);
    auto criticalHp = int(0.15 * (float)maxHp);

    return currentHp <= criticalHp;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::IsBeingHit()
{
    if (!IsControllingEntity() ||
        !EntityExists())
        return false;

    return Entity()->Attr(EOATTR_IsBeingHit) > 0;
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

    int los = Entity()->Type()->Attr(ECATTR_LineOfSight);
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
    int los = Entity()->Type()->Attr(ECATTR_LineOfSight);

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

    int los = Entity()->Type()->Attr(ECATTR_LineOfSight);
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