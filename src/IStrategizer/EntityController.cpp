#include "EntityController.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameEntity.h"
#include "GameType.h"
#include "IMSystemManager.h"
#include "GroundControlIM.h"
#include "EntityFSM.h"
#include "ArmyController.h"
#include "MessagePump.h"

using namespace IStrategizer;

EntityController::EntityController(ArmyController* pController) :
m_entityId(INVALID_TID),
m_targetEntityId(INVALID_TID),
m_singleTargetPos(Vector2::Inf()),
m_pController(pController)
{
    g_MessagePump->RegisterForMessage(MSG_EntityDestroy, this);
}

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
TID EntityController::TargetEntity() const
{
    // If my target entity is not set, always return the army target position
    // If my target position is set, then return my override position
    // otherwise if I am part of an army, then return controller position, otherwise
    // return my position
    if (m_targetEntityId == INVALID_TID)
        return (m_pController != nullptr ? m_pController->TargetEntity() : m_targetEntityId);
    else
        return m_targetEntityId;
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
    }
}
//////////////////////////////////////////////////////////////////////////
void EntityController::Update()
{
    if (m_entityId == INVALID_TID)
        return;

    m_pLogicMemory.top()->Update();
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
bool EntityController::IsOnCriticalHP() const
{
    if (!IsControllingEntity() ||
        !EntityExists())
        return false;

    return IsOnCriticalHP(Entity());
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::IsBeingHit() const
{
    if (!IsControllingEntity() ||
        !EntityExists())
        return false;

    return Entity()->P(OP_IsBeingHit) > 0;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::ArrivedAtTarget(_In_ Vector2 pos) const
{
    if (!IsControllingEntity() ||
        !EntityExists() ||
        pos.IsInf())
        return false;

    auto distToTarget = pos.Distance(Entity()->Position());

    return distToTarget <= PositionArriveRadius;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::ThreatAtTarget(_In_ Vector2 pos) const
{
    if (!IsControllingEntity() ||
        !EntityExists() ||
        pos.IsInf())
        return false;

    auto pGrnCtrlIM = (GroundControlIM*)g_IMSysMgr.GetIM(IM_GroundControl);
    return pGrnCtrlIM->GetCellInfluenceFromWorldPosition(pos) < 0;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::IsTargetInSight(_In_ Vector2 pos) const
{
    if (!IsControllingEntity() ||
        !EntityExists() ||
        pos.IsInf())
        return false;

    int los = Entity()->Type()->P(TP_LineOfSight);
    Circle2 sight(Entity()->Position(), los);

    return sight.IsInside(pos);
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::IsTargetInSight(_In_ TID entityId) const
{
    if (!IsControllingEntity() ||
        !EntityExists())
        return false;

    auto pEntity = g_Game->GetEntity(entityId);
    return IsTargetInSight(pEntity->Position());
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
    Vector2 selfPos = Entity()->Position();
    Vector2 otherPos = Vector2::Inf();
    int los = Entity()->Type()->P(TP_LineOfSight);

    // For now, just select the closest entity in sight
    for (auto& entityR : g_Game->Enemy()->Entities())
    {
        if (!Entity()->CanAttack(entityR.first))
            continue;

        otherPos = entityR.second->Position();
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
bool EntityController::IsAnyEnemyTargetInSight() const
{
    if (!IsControllingEntity() ||
        !EntityExists())
        return false;

    EntityList enemies;

    int los = Entity()->Type()->P(TP_LineOfSight);
    Circle2 sight(Entity()->Position(), los);

    for (auto& entityR : g_Game->Enemy()->Entities())
    {
        if (sight.IsInside(entityR.second->Position()) &&
            Entity()->CanAttack(entityR.first))
            return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::IsAnyEnemyTargetInRange() const
{
    if (!IsControllingEntity() ||
        !EntityExists())
        return false;

    EntityList enemies;

    int range = Entity()->Type()->P(TP_GroundRange);
    Circle2 rangeArea(Entity()->Position(), range);

    for (auto& entityR : g_Game->Enemy()->Entities())
    {
        if (rangeArea.IsInside(entityR.second->Position()))
            return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::EntityExists(_In_ TID entityId) const
{
    if (entityId == INVALID_TID)
        return false;

    auto pEntity = g_Game->GetEntity(entityId);
    return pEntity != nullptr && pEntity->Exists();
}
//////////////////////////////////////////////////////////////////////////
void EntityController::OnEntityFleeing()
{
    if (m_pController != nullptr)
    {
        if (EntityExists())
        {
            LogInfo("%s is fleeing!", Entity()->ToString().c_str());
        }

        m_pController->OnEntityFleeing(m_entityId);
    }
}
//////////////////////////////////////////////////////////////////////////
void EntityController::HardResetLogic()
{
    while (!m_pLogicMemory.empty())
        m_pLogicMemory.pop();

    m_pLogicMemory.push(StackFSMPtr(new IdleEntityFSM(this)));
}
//////////////////////////////////////////////////////////////////////////
void EntityController::PushIdleLogic()
{
    PushLogic(StackFSMPtr(new IdleEntityFSM(this)));
}
//////////////////////////////////////////////////////////////////////////
TID EntityController::Attacker() const
{
    // Finding my attack can be expensive, thats why it is globally computed per army
    if (m_pController == nullptr)
        DEBUG_THROW(NotImplementedException(XcptHere));

    auto& allEnemies = m_pController->EnemyData();
    auto& nearEnemies = m_pController->ClosestEnemyEntities();

    int minDist = INT_MAX;
    TID closestAttacker = INVALID_TID;
    auto selfPos = Entity()->Position();

    for (auto& enemy : nearEnemies)
    {
        auto& currEnemy = allEnemies.at(enemy.second);

        auto pCurrEnemy = g_Game->Enemy()->GetEntity(currEnemy.Id);

        if (currEnemy.TargetEntityId == m_entityId &&
            pCurrEnemy->P(OP_IsAttacking))
        {
            int dist = selfPos.Distance(pCurrEnemy->Position());
            if (dist < minDist)
            {
                minDist = dist;
                closestAttacker = currEnemy.Id;
            }
        }
    }

    return closestAttacker;
}
//////////////////////////////////////////////////////////////////////////
bool EntityController::IsCloseToMeleeAttacker() const
{
    // Finding my attack can be expensive, thats why it is globally computed per army
    if (m_pController == nullptr)
        DEBUG_THROW(NotImplementedException(XcptHere));

    auto& allEnemies = m_pController->EnemyData();
    auto& nearEnemies = m_pController->ClosestEnemyEntities();

    int minDist = INT_MAX;
    TID closestAttacker = INVALID_TID;
    auto selfPos = Entity()->Position();

    for (auto& enemy : nearEnemies)
    {
        auto& currEnemy = allEnemies.at(enemy.second);
        auto pCurrEnemy = g_Game->Enemy()->GetEntity(currEnemy.Id);

        int dist = selfPos.Distance(pCurrEnemy->Position());
        if (dist < minDist)
        {
            minDist = dist;
            closestAttacker = currEnemy.Id;
        }
    }

    if (closestAttacker != INVALID_TID)
    {
        auto pCurrEnemy = g_Game->Enemy()->GetEntity(closestAttacker);

        if (pCurrEnemy->Type()->P(TP_IsMelee) && minDist <= MeleeAttackerSafetyRadius)
            return true;
        else
            return false;
    }
    else
        return false;
}