#include "EntityFSM.h"
#include "EntityController.h"
#include "GamePlayer.h"
#include "GameEntity.h"
#include "GameType.h"

using namespace IStrategizer;
using namespace std;

//////////////////////////////////////////////////////////////////////////
//
// States
//
void EntityState::Enter()
{
    auto pController = (EntityController*)m_pController;
    LogInfo("%s: %s -> Enter", pController->Entity()->ToString(true).c_str(), ToString().c_str());

    m_targetPos1 = pController->TargetPosition();
    m_targetEntity = pController->TargetEntity();
}
//////////////////////////////////////////////////////////////////////////
void EntityState::Exit()
{
    m_targetPos1 = Vector2::Inf();
    m_targetEntity = INVALID_TID;

    auto pController = (EntityController*)m_pController;
    LogInfo("%s: %s -> Exit", pController->Entity()->ToString(true).c_str(), ToString().c_str());
}
//////////////////////////////////////////////////////////////////////////
void EntityState::Update()
{
    auto pController = (EntityController*)m_pController;
    pController->Entity()->DebugDrawTarget();
    pController->Entity()->DebugDrawHealthBar();

    string str = ToString();
    str += '_';
    str += to_string(pController->Entity()->Id());

    g_Game->DebugDrawMapText(pController->Entity()->Position(), str);
}
//////////////////////////////////////////////////////////////////////////
void ArriveEntityState::Update()
{
    EntityState::Update();

    g_Game->DebugDrawMapCircle(m_targetPos1, EntityController::PositionArriveRadius, GCLR_Yellow);

    if (g_Game->GameFrame() % 4 != 0)
        return;

    auto pController = (EntityController*)m_pController;
    if (!pController->Entity()->P(OP_IsMoving))
    {
        m_targetPos1 = Circle2(m_targetPos1, EntityController::PositionArriveRadius).RandomInside();
        pController->Entity()->Move(m_targetPos1);
    }
}
//////////////////////////////////////////////////////////////////////////
void FleeEntityState::Enter()
{
    EntityState::Enter();

    auto pController = (EntityController*)m_pController;
    pController->OnEntityFleeing();
}
//////////////////////////////////////////////////////////////////////////
void FleeEntityState::Update()
{
    EntityState::Update();

    g_Game->DebugDrawMapCircle(m_targetPos1, EntityController::PositionArriveRadius, GCLR_Yellow);

    if (g_Game->GameFrame() % 4 != 0)
        return;

    auto pController = (EntityController*)m_pController;
    if (!pController->Entity()->P(OP_IsMoving))
    {
        m_targetPos1 = Circle2(g_Game->Self()->StartLocation(), EntityController::PositionArriveRadius).RandomInside();
        pController->Entity()->Move(m_targetPos1);
    }
}
//////////////////////////////////////////////////////////////////////////
void AttackEntityState::Update()
{
    EntityState::Update();


    if (g_Game->GameFrame() % 4 != 0)
        return;

    TID prevTargetEntityId = m_targetEntity;

    auto pController = (EntityController*)m_pController;
    m_targetEntity = pController->TargetEntity();

    // When the unit can attack the target and it is not attacking or
    // it is attacking but a different target, then order it to attack our target
    if (m_targetEntity != prevTargetEntityId &&
        m_targetEntity != INVALID_TID &&
        pController->Entity()->CanAttack(m_targetEntity))
    {
        LogInfo("%s choose to attack %s", pController->Entity()->ToString().c_str(), g_Game->GetEntity(m_targetEntity)->ToString().c_str());
        pController->Entity()->AttackEntity(m_targetEntity);
    }
}
//////////////////////////////////////////////////////////////////////////
void AlarmEntityState::Update()
{
    EntityState::Update();

    auto pController = (EntityController*)m_pController;

    g_Game->DebugDrawMapLine(pController->Entity()->Position(), m_targetPos1, GCLR_Orange);
}
//////////////////////////////////////////////////////////////////////////
void RetreatEntityState::Reset()
{
    EntityState::Reset();
    m_retreatAttackerId = INVALID_TID;
    m_retreatPos = Vector2::Inf();
}
//////////////////////////////////////////////////////////////////////////
void RetreatEntityState::Update()
{
    EntityState::Update();

    auto pController = (EntityController*)m_pController;
    m_retreatAttackerId = pController->Attacker();

    if (m_retreatAttackerId != INVALID_TID)
    {
        auto attackerPos = g_Game->Enemy()->GetEntity(m_retreatAttackerId)->Position();
        auto selfPos = pController->Entity()->Position();
        Vector2F attackerPosF((float)attackerPos.X, (float)attackerPos.Y);
        Vector2F selfPosF((float)selfPos.X, (float)selfPos.Y);

        auto retreatkDir = selfPosF - attackerPosF;
        retreatkDir.Normalize();

        float retreatDistance = (float)pController->Entity()->Type()->P(TP_GroundRange) * 1.5f;

        Vector2F newPos = selfPosF + retreatkDir * retreatDistance;

        pController->Entity()->Move(Vector2((int)newPos.X, (int)newPos.Y));
    }

    pController->Entity()->DebugDrawRange();
}
//////////////////////////////////////////////////////////////////////////
//
// Machines
//
void ScoutEntityFSM::CheckTransitions()
{
    auto pController = (EntityController*)m_pController;

    // Get target position in case the current state is using it
    auto pCurrState = static_pointer_cast<EntityState>(CurrentState());
    Vector2 targetPos = pCurrState->TargetPosition();

    switch (pCurrState->TypeId())
    {
    case ArriveEntityState::TypeID:
        if (pController->IsOnCriticalHP() ||
            (m_scoutGoal == SCTGL_Explore && pController->IsTargetInSight(targetPos) && pController->ThreatAtTarget(targetPos)))
        {
            PopState();
            PushState(FleeEntityState::TypeID);
        }
        // Pick next target
        else if ((m_scoutGoal == SCTGL_Visit && pController->ArrivedAtTarget(targetPos)) ||
            (m_scoutGoal == SCTGL_Explore && pController->IsTargetInSight(targetPos) && !pController->ThreatAtTarget(targetPos)))
        {
            if (SetNextTargetPosition())
            {
                PopState();
                PushState(ArriveEntityState::TypeID);
            }
            else
            {
                PopState();
                PushState(FleeEntityState::TypeID);
            }
        }
        break;
    case FleeEntityState::TypeID:
        if (pController->ArrivedAtTarget(targetPos))
        {
            PopState();
            PushState(IdleEntityState::TypeID);
        }
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
bool ScoutEntityFSM::SetNextTargetPosition()
{
    if (MoreTargetsExist())
    {
        ++m_currTargetPosIdx;
        auto pController = (EntityController*)m_pController;
        pController->TargetPosition(pController->MultiTargetPosition()[m_currTargetPosIdx]);

        return true;
    }
    else
        return false;
}
//////////////////////////////////////////////////////////////////////////
bool ScoutEntityFSM::MoreTargetsExist()
{
    auto pController = (EntityController*)m_pController;
    return m_currTargetPosIdx + 1 < (int)pController->MultiTargetPosition().size();
}
//////////////////////////////////////////////////////////////////////////
void ScoutEntityFSM::Reset()
{
    StackFSM::Reset();

    auto pController = (EntityController*)m_pController;
    pController->TargetPosition(pController->MultiTargetPosition()[0]);
    m_currTargetPosIdx = 0;
}
//////////////////////////////////////////////////////////////////////////
void GuardEntityFSM::CheckTransitions()
{
    auto pController = (EntityController*)m_pController;

    // Get target position in case the current state is using it
    auto pCurrState = static_pointer_cast<EntityState>(CurrentState());
    Vector2 targetPos = pCurrState->TargetPosition();

    switch (pCurrState->TypeId())
    {
    case FleeEntityState::TypeID:
        if (pController->ArrivedAtTarget(targetPos))
        {
            PopAllAndPushState(IdleEntityState::TypeID);
        }
        break;
    case ArriveEntityState::TypeID:
        if (pController->IsOnCriticalHP())
        {
            PopAllAndPushState(FleeEntityState::TypeID);
        }
        else if (pController->ArrivedAtTarget(targetPos))
        {
            PopState();
        }
        break;
    case AttackEntityState::TypeID:

        if (pController->IsOnCriticalHP())
        {
            PopAllAndPushState(FleeEntityState::TypeID);
        }
        else if (pController->TargetEntity() == INVALID_TID)
        {

        }
        else if (!pController->IsTargetInSight(pCurrState->TargetEntity()) ||
            !pController->EntityExists(pCurrState->TargetEntity()))
        {
            PopState();
        }
        break;
    case AlarmEntityState::TypeID:
        if (!pController->IsTargetInSight(targetPos))
        {
            PushState(ArriveEntityState::TypeID);
        }
        else if (pController->IsAnyEnemyTargetInSight())
        {
            PushState(AttackEntityState::TypeID);
        }
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
void HintNRunEntityFSM::CheckTransitions()
{
    auto pController = (EntityController*)m_pController;

    // Get target position in case the current state is using it
    auto pCurrState = static_pointer_cast<EntityState>(CurrentState());
    Vector2 targetPos = pCurrState->TargetPosition();

    switch (pCurrState->TypeId())
    {
    case FleeEntityState::TypeID:
        if (pController->ArrivedAtTarget(targetPos))
        {
            PopAllAndPushState(IdleEntityState::TypeID);
        }
        break;
    case AttackEntityState::TypeID:
        if (pController->IsOnCriticalHP())
        {
            PopState();
            PushState(FleeEntityState::TypeID);
        }
        else if (pController->TargetEntity() == INVALID_TID &&
            !pController->IsAnyEnemyTargetInSight())
        {
            PopState();
        }
        else if (pController->Entity()->P(OP_IsBeingHit))
        {
            PushState(RetreatEntityState::TypeID);
        }
        break;
    case RetreatEntityState::TypeID:
        if (!pController->IsAnyEnemyTargetInRange())
        {
            PopState();
        }
        break;
    case AlarmEntityState::TypeID:
        if (pController->IsOnCriticalHP())
        {
            PopState();
            PushState(FleeEntityState::TypeID);
        }
        else if (pController->TargetEntity() != INVALID_TID ||
            pController->IsAnyEnemyTargetInSight())
        {
            PushState(AttackEntityState::TypeID);
        }
        break;
    }
}