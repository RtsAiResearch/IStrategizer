#include "ScFSM.h"
#include "EntityController.h"
#include "GameEntity.h"
#include "RtsGame.h"
#include "ScStrategyManager.h"

using namespace IStrategizer;
using namespace std;

void PlantSpiderMinesState::Enter()
{
    EntityState::Enter();

    auto pController = (EntityController*)m_pController;

    auto pTileUnits = g_GameImpl->MapUnitsOnTile(pController->Entity()->Position());

    bool anyMineOnTile = false;

    for (int i = 0; i < pTileUnits->Size(); ++i)
    {
        if (g_GameImpl->UnitGetType(pTileUnits->At(i))->GameId() == ScStrategyManager::SpiderMine->GameId())
        {
            anyMineOnTile = true;
            break;
        }
    }

    if (g_GameImpl->UnitCanUseTechPosition(pController->EntityId(),
        ScStrategyManager::SpiderMine,
        g_GameImpl->UnitTilePosition(pController->EntityId())))
    {
        g_GameImpl->UnitUseTechPosition(pController->EntityId(),
            ScStrategyManager::SpiderMine,
            g_GameImpl->UnitTilePosition(pController->EntityId()));
    }
}
//////////////////////////////////////////////////////////////////////////
void VultureHintNRunEntityFSM::CheckTransitions()
{
    // Get target position in case the current state is using it
    auto pCurrState = static_pointer_cast<EntityState>(CurrentState());
    Vector2 targetPos = pCurrState->TargetPosition();
    auto pController = (EntityController*)m_pController;

    switch (pCurrState->TypeId())
    {
    case FleeEntityState::TypeID:
        if (pController->ArrivedAtTarget(targetPos))
        {
            PopState();
            PushState(IdleEntityState::TypeID);
        }
        break;
    case AttackEntityState::TypeID:
        if (pController->IsOnCriticalHP())
        {
            PopState();
            PushState(FleeEntityState::TypeID);
        }
        else if (pController->TargetEntity() == INVALID_TID)
        {
            PopState();
        }
        else if ((pController->Entity()->P(OP_IsBeingHit) &&
            pController->Entity()->HitpointsPercentage() < EntityController::HealthyHpPercent) ||
            pController->IsCloseToMeleeAttacker())
        {
            PushState(RetreatEntityState::TypeID);
        }
        break;
    case RetreatEntityState::TypeID:
        if (pController->Attacker() == INVALID_TID &&
            pController->CloseMeleeAttacker() == INVALID_TID)
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
        else if (pController->TargetEntity() == INVALID_TID)
        {
            PushState(PlantSpiderMinesState::TypeID);
        }
        else if (pController->TargetEntity() != INVALID_TID)
        {
            PushState(AttackEntityState::TypeID);
        }
        break;
    case PlantSpiderMinesState::TypeID:
        PopState();
        break;
    }
}