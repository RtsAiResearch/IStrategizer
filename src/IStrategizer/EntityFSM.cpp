#include "EntityFSM.h"
#include "EntityController.h"
#include "GamePlayer.h"
#include "GameEntity.h"

using namespace IStrategizer;

//////////////////////////////////////////////////////////////////////////
//
// States
//
void ArriveState::Enter()
{
    auto pController = (EntityController*)m_pController;
    m_targetPos = pController->TargetPosition();
    bool success = pController->Entity()->Move(m_targetPos);
    _ASSERTE(success);
}
//////////////////////////////////////////////////////////////////////////
void ArriveState::Update()
{
    auto pController = (EntityController*)m_pController;
    g_Game->DrawMapLine(pController->Entity()->GetPosition(), m_targetPos, GCLR_Yellow);
    g_Game->DrawMapCircle(m_targetPos, EntityController::PositionArriveRadius, GCLR_Blue);
}
//////////////////////////////////////////////////////////////////////////
void FleeState::Enter()
{
    auto pController = (EntityController*)m_pController;
    m_targetPos = g_Game->Self()->StartLocation();
    bool success = pController->Entity()->Move(m_targetPos);
    _ASSERTE(success);
}
//////////////////////////////////////////////////////////////////////////
void FleeState::Update()
{
    auto pController = (EntityController*)m_pController;
    g_Game->DrawMapLine(pController->Entity()->GetPosition(), m_targetPos, GCLR_Yellow);
    g_Game->DrawMapCircle(m_targetPos, EntityController::PositionArriveRadius, GCLR_Blue);
}
//////////////////////////////////////////////////////////////////////////
//
// Machines
//
void ScoutEntityFSM::CheckTransitions()
{
    auto pController = (EntityController*)m_pController;

    switch (CurrentState()->TypeId())
    {
    case ArriveState::TypeID:
        if (pController->IsOnCriticalHP() ||
            (m_scoutGoal == SCTGL_Explore && pController->IsTargetInSight() && pController->ThreatAtTarget()))
        {
            PopState();
            PushState(FleeState::TypeID);
        }
        // Pick next target
        else if ((m_scoutGoal == SCTGL_Visit && pController->ArrivedAtTarget()) ||
            (m_scoutGoal == SCTGL_Explore && pController->IsTargetInSight() && !pController->ThreatAtTarget()))
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
        if (pController->ArrivedAtTarget())
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