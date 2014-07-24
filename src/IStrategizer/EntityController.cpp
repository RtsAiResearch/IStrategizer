#include "EntityController.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameEntity.h"
#include "GameType.h"

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

    m_targetPos = Vector2::Inf();
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
bool EntityController::EntityExist() const
{
    return g_Game->Self()->GetEntity(m_entityId) != nullptr;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::IsOnCriticalHP()
{
    if (!IsControllingEntity() ||
        !EntityExist())
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
        !EntityExist())
        return false;

    auto pEntity = g_Game->Self()->GetEntity(m_entityId);
    return pEntity->Attr(EOATTR_IsBeingHit) > 0;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::ArrivedAtTarget()
{
    if (!IsControllingEntity() ||
        !EntityExist() ||
        m_targetPos.IsInf())
        return false;

    auto pEntity = g_Game->Self()->GetEntity(m_entityId);
    auto pos = pEntity->GetPosition();
    auto distToTarget = m_targetPos.Distance(pos);

    return distToTarget <= PositionArriveRadius;
}