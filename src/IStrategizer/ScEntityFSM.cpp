#include "ScEntityFSM.h"
#include "EntityController.h"
#include "GameEntity.h"
#include "RtsGame.h"
#include "ScStrategyManager.h"
#include "WorldMap.h"

using namespace IStrategizer;
using namespace std;

int PlantSpiderMinesState::MinesCountInRegion(_In_ Vector2 pos)
{
    auto pTileUnits = g_GameImpl->MapUnitsInRegion(pos);

    int count = 0;

    for (int i = 0; i < pTileUnits->Size(); ++i)
    {
        if (g_GameImpl->UnitGetType(pTileUnits->At(i))->GameId() == ScStrategyManager::SpiderMine->GameId())
        {
            ++count;
        }
    }

    return count;
}
//////////////////////////////////////////////////////////////////////////
bool PlantSpiderMinesState::IsMineOnTile(_In_ TID entityId)
{
    auto pTileUnits = g_GameImpl->MapUnitsOnTile(TilePositionFromUnitPosition(g_GameImpl->UnitPosition(entityId)));

    bool anyMineOnTile = false;

    for (int i = 0; i < pTileUnits->Size(); ++i)
    {
        if (g_GameImpl->UnitGetType(pTileUnits->At(i))->GameId() == ScStrategyManager::SpiderMine->GameId())
        {
            anyMineOnTile = true;
            break;
        }
    }

    return anyMineOnTile;
}
//////////////////////////////////////////////////////////////////////////
void PlantSpiderMinesState::Enter()
{
    EntityState::Enter();

    auto pController = (EntityController*)m_pController;
    Vector2 minePos;
    
    if (pController->TargetEntity() != INVALID_TID)
        minePos = g_GameImpl->UnitPosition(pController->TargetEntity());
    else
    {
        _ASSERTE(pController->TargetPosition() != Vector2::Inf());
        minePos = Circle2(pController->TargetPosition(), 128).RandomInside();
    }

    if (g_GameImpl->UnitCanUseTechPosition(pController->EntityId(),
        ScStrategyManager::SpiderMine,
        minePos))
    {
        if (g_GameImpl->UnitUseTechPosition(pController->EntityId(),
            ScStrategyManager::SpiderMine,
            minePos))
        {
            LogInfo("%s is planting mine at %s", pController->Entity()->ToString().c_str(), minePos.ToString().c_str());
        }
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
        else if (pController->Entity()->P(OP_IsBeingHit) ||
            pController->IsCloseToMeleeAttacker())
        {
            PushState(RetreatEntityState::TypeID);
        }
        else if (g_GameImpl->GameFrame() % 25 == 0 &&
            !PlantSpiderMinesState::IsMineOnTile(pController->TargetEntity()))
        {
            PushState(PlantSpiderMinesState::TypeID);
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
        else if (pController->TargetEntity() != INVALID_TID)
        {
            PushState(AttackEntityState::TypeID);
        }
        else if (g_GameImpl->GameFrame() % 50 == 0 &&
            PlantSpiderMinesState::MinesCountInRegion(pController->TargetPosition()) < 4)
        {
            PushState(PlantSpiderMinesState::TypeID);
        }
        break;
    case PlantSpiderMinesState::TypeID:
        if (!g_GameImpl->UnitIsPlantingMine(pController->EntityId()))
        {
            PopState();
        }
        break;
    }
}