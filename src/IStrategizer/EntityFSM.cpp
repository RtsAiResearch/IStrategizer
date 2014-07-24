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
    bool success = pController->Entity()->Move(pController->TargetPosition());
    _ASSERTE(success);
}
//////////////////////////////////////////////////////////////////////////
void FleeState::Enter()
{
    auto pController = (EntityController*)m_pController;
    bool success = pController->Entity()->Move(g_Game->Self()->StartLocation());
    _ASSERTE(success);
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
            pController->ArrivedAtTarget())
        {
            PopState();
            PushState(FleeState::TypeID);
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
