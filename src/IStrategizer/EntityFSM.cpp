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
void EntityState::Update()
{
    auto pController = (EntityController*)m_pController;
    pController->Entity()->DebugDrawTarget();
    pController->Entity()->DebugDrawRange();
    pController->Entity()->DebugDrawLineOfSight();
    g_Game->DebugDrawMapText(pController->Entity()->GetPosition(), ToString().c_str());
}
//////////////////////////////////////////////////////////////////////////
void ArriveState::Enter()
{
    auto pController = (EntityController*)m_pController;
    m_targetPos1 = pController->TargetPosition();
    bool success = pController->Entity()->Move(m_targetPos1);
    _ASSERTE(success);
}
//////////////////////////////////////////////////////////////////////////
void ArriveState::Update()
{
    EntityState::Update();

    g_Game->DebugDrawMapCircle(m_targetPos1, EntityController::PositionArriveRadius, GCLR_Yellow);

    auto pController = (EntityController*)m_pController;
    if (pController->Entity()->Attr(EOATTR_State) == OBJSTATE_Idle)
    {
        LogInfo("Entity is idle while it is supposed to be moving, retrying the command");
        pController->Entity()->Move(m_targetPos1);
    }
}
//////////////////////////////////////////////////////////////////////////
void FleeState::Enter()
{
    auto pController = (EntityController*)m_pController;
    m_targetPos1 = g_Game->Self()->StartLocation();
    bool success = pController->Entity()->Move(m_targetPos1);
    _ASSERTE(success);
}
//////////////////////////////////////////////////////////////////////////
void FleeState::Update()
{
    EntityState::Update();

    g_Game->DebugDrawMapCircle(m_targetPos1, EntityController::PositionArriveRadius, GCLR_Yellow);

    auto pController = (EntityController*)m_pController;
    if (pController->Entity()->Attr(EOATTR_State) == OBJSTATE_Idle)
    {
        LogInfo("Entity is idle while it is supposed to be moving, retrying the command");
        pController->Entity()->Move(m_targetPos1);
    }
}
//////////////////////////////////////////////////////////////////////////
void AttackState::Enter()
{
    auto pController = (EntityController*)m_pController;
    m_targetEntity = pController->SmartSelectEnemyEntityInSight();
    bool success = pController->Entity()->AttackEntity(m_targetEntity);
    _ASSERTE(success);
}
//////////////////////////////////////////////////////////////////////////
void AttackState::Update()
{
    EntityState::Update();

    auto pController = (EntityController*)m_pController;
    if (pController->Entity()->Attr(EOATTR_State) == OBJSTATE_Idle)
    {
        auto pTarget = g_Game->GetEntity(m_targetEntity);
        if (pTarget->Exists())
        {
            LogInfo("Entity is idle while it is supposed to be attacking, retrying the command");
            pController->Entity()->AttackEntity(m_targetEntity);
        }
    }

    g_Game->DebugDrawMapLine(pController->Entity()->GetPosition(), m_targetPos1, GCLR_Orange);
}
//////////////////////////////////////////////////////////////////////////
void AlarmState::Enter()
{
    auto pController = (EntityController*)m_pController;
    m_targetPos1 = pController->TargetPosition();
}
//////////////////////////////////////////////////////////////////////////
void AlarmState::Update()
{
    auto pController = (EntityController*)m_pController;
    g_Game->DebugDrawMapLine(pController->Entity()->GetPosition(), m_targetPos1, GCLR_Orange);
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
    Vector2 targetPos = pCurrState->TargetPosition1();

    switch (pCurrState->TypeId())
    {
    case ArriveState::TypeID:
        if (pController->IsOnCriticalHP() ||
            (m_scoutGoal == SCTGL_Explore && pController->IsTargetInSight(targetPos) && pController->ThreatAtTarget(targetPos)))
        {
            PopState();
            PushState(FleeState::TypeID);
        }
        // Pick next target
        else if ((m_scoutGoal == SCTGL_Visit && pController->ArrivedAtTarget(targetPos)) ||
            (m_scoutGoal == SCTGL_Explore && pController->IsTargetInSight(targetPos) && !pController->ThreatAtTarget(targetPos)))
        {
            if (SetNextTargetPosition())
            {
                PopState();
                PushState(ArriveState::TypeID);
            }
            else
            {
                PopState();
                PushState(FleeState::TypeID);
            }
        }
        break;
    case FleeState::TypeID:
        if (pController->ArrivedAtTarget(targetPos))
        {
            PopState();
            PushState(IdleState::TypeID);
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
}
//////////////////////////////////////////////////////////////////////////
void GuardEntityFSM::CheckTransitions()
{
    auto pController = (EntityController*)m_pController;

    // Get target position in case the current state is using it
    auto pCurrState = static_pointer_cast<EntityState>(CurrentState());
    Vector2 targetPos = pCurrState->TargetPosition1();

    switch (pCurrState->TypeId())
    {
    case FleeState::TypeID:
        if (pController->ArrivedAtTarget(targetPos))
        {
            PopAllAndPushState(IdleState::TypeID);
        }
        break;
    case ArriveState::TypeID:
        if (pController->IsOnCriticalHP())
        {
            PopAllAndPushState(FleeState::TypeID);
        }
        else if (pController->ArrivedAtTarget(targetPos))
        {
            PopState();
        }
        break;
    case AttackState::TypeID:
        if (pController->IsOnCriticalHP())
        {
            PopAllAndPushState(FleeState::TypeID);
        }
        else if (!pController->IsTargetInSight(pCurrState->TargetEntity()) ||
            !pController->EntityExists(pCurrState->TargetEntity()))
        {
            PopState();
        }
        break;
    case AlarmState::TypeID:
        if (!pController->IsTargetInSight(targetPos))
        {
            PushState(ArriveState::TypeID);
        }
        else if (pController->IsAnyTargetInSight())
        {
            PushState(AttackState::TypeID);
        }
        break;
    }
}