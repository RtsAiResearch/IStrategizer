#include "EntityController.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameEntity.h"
#include "GameType.h"
#include "IMSystemManager.h"
#include "GroundControlIM.h"

using namespace IStrategizer;

void EntityController::ControlEntity(_In_ TID entityId, _In_ StackFSMPtr pLogic)
{
    if (m_entityId != INVALID_TID)
        ReleaseEntity();

    m_entityId = entityId;
    m_pLogic = pLogic;

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

    m_multiTargetPos[0] = Vector2::Inf();
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
    return g_Game->Self()->GetEntity(m_entityId) != nullptr;
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

    auto pEntity = g_Game->Self()->GetEntity(m_entityId);
    return pEntity->Attr(EOATTR_IsBeingHit) > 0;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::ArrivedAtTarget(_In_ Vector2 pos)
{
    if (!IsControllingEntity() ||
        !EntityExists() ||
        pos.IsInf())
        return false;

    auto pEntity = g_Game->Self()->GetEntity(m_entityId);
    auto distToTarget = pos.Distance(pEntity->GetPosition());

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

    auto pEntity = g_Game->Self()->GetEntity(m_entityId);
    int los = pEntity->Type()->Attr(ECATTR_LineOfSight);
    Circle2 sight(pEntity->GetPosition(), los);

    return sight.IsInside(pos);
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::IsTargetInSight(_In_ TID entityId)
{
    if (!IsControllingEntity() ||
        !EntityExists())
        return false;

    auto pEntity = g_Game->Self()->GetEntity(m_entityId);
    return IsTargetInSight(pEntity->GetPosition());
}
//////////////////////////////////////////////////////////////////////////
TID EntityController::SmartSelectEnemyEntityInSight()
{
    return INVALID_TID;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::IsAnyTargetInSight()
{
    return true;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::EntityExists(_In_ TID entityId) const
{
    auto pEntity = g_Game->GetEntity(entityId);
    return pEntity != nullptr && pEntity->Exists();
}