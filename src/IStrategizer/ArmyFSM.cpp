#include "ArmyFSM.h"
#include "ArmyController.h"
#include "EntityFSM.h"
#include "GameEntity.h"
#include "WorldMap.h"

using namespace IStrategizer;
using namespace std;

//////////////////////////////////////////////////////////////////////////
//
// States
//
//////////////////////////////////////////////////////////////////////////
void ArmyState::Enter()
{
    LogInfo("%s -> Enter", ToString().c_str());

    auto pController = (ArmyController*)m_pController;
    m_controlledEntities = pController->Entities();
    m_targetPos1 = pController->TargetPosition();
    m_targetEntity = pController->TargetEntity();

    for (auto& entityR : m_controlledEntities)
    {
        auto pMicroLogic = pController->Consultant()->SelectMicroLogic(pController, &*entityR.second);
        entityR.second->PushLogic(pMicroLogic);
    }

    LogInfo("%s with %d controlled entities", ToString().c_str(), m_controlledEntities.size());
}
//////////////////////////////////////////////////////////////////////////
void ArmyState::Exit()
{
    for (auto& entityR : m_controlledEntities)
    {
        entityR.second->PopLogic();
    }

    LogInfo("%s with %d controlled entities", ToString().c_str(), m_controlledEntities.size());
    LogInfo("%s -> Exit", ToString().c_str());
}
//////////////////////////////////////////////////////////////////////////
void ArmyState::DebugDraw()
{
    auto pController = (ArmyController*)m_pController;

    if (!pController->TargetPosition().IsInf())
        g_Game->DebugDrawMapLine(pController->Center(), pController->TargetPosition(), GCLR_Orange);

    if (pController->TargetEntity() != INVALID_TID &&
        g_Game->GetEntity(pController->TargetEntity()) != nullptr)
        g_Game->DebugDrawMapCircle(g_Game->GetEntity(pController->TargetEntity())->Position(), 16, GCLR_Red);

    g_Game->DebugDrawMapCircle(pController->Center(), 32, GCLR_Orange);

    // <army-fsm-str><fsm-state-str><num-controlled>/<num-died>
    char stats[128];
    auto statsPos = pController->Center();

    sprintf_s(stats, "%s",
        ToString().c_str());
    g_Game->DebugDrawMapText(statsPos, stats);

    sprintf_s(stats, "C:%d D:%d HP:%d GA:%d", pController->Entities().size(),
        pController->TotalDiedEntities(), pController->TotalMaxHP(), pController->TotalGroundAttack());

    statsPos.Y += 10;
    g_Game->DebugDrawMapText(statsPos, stats);

    auto focusArea = pController->FormationArea();
    g_Game->DebugDrawMapCircle(focusArea.Center, focusArea.Radius, GCLR_Blue);

    auto sightArea = pController->SightArea();
    g_Game->DebugDrawMapCircle(sightArea.Center, sightArea.Radius, GCLR_Cyan);
}
//////////////////////////////////////////////////////////////////////////
void ArmyState::Update()
{
    // Remove non-existing entities
    for (EntityControllersMap::iterator entitryRItr = m_controlledEntities.begin();
        entitryRItr != m_controlledEntities.end();)
    {
        if (!entitryRItr->second->EntityExists())
            m_controlledEntities.erase(entitryRItr++);
        else
            ++entitryRItr;
    }
}
//////////////////////////////////////////////////////////////////////////
void RegroupArmyState::Enter()
{
    ArmyState::Enter();

    m_formation.Reset();

    auto pController = (ArmyController*)m_pController;

    if (m_regroupAtArmyCenter)
    {
        TID closestToCenterId = pController->ClosestEntityToCenter();
        m_targetPos1 = g_Game->Map()->GetClosestReachableRegionCenter(closestToCenterId);
    }
    else
        m_targetPos1 = pController->TargetPosition();

    m_formation.Center = TargetPosition();
    pController->CalcGroupFormation(m_formation);
    m_regroupArea = Circle2(TargetPosition(), pController->FormationData().CircleRadius);
}
//////////////////////////////////////////////////////////////////////////
void RegroupArmyState::DebugDraw()
{
    g_Game->DebugDrawMapCircle(m_regroupArea.Center, m_regroupArea.Radius, GCLR_Purple);

    for (auto& p : m_formation.Placement)
    {
        g_Game->DebugDrawMapRectangle(p.second, p.second + ArmyController::FormationSpacing, GCLR_Blue, false);
    }
}
//////////////////////////////////////////////////////////////////////////
void RegroupArmyState::Update()
{
    ArmyState::Update();

    for (auto& entityR : m_controlledEntities)
    {
        auto pEntity = entityR.second->Entity();

        // Re issue move to any astray unit
        // 1- Outside the formation area
        // 2- Not moving and outside regroup area
        // 3- Moving but its target pos is not its designated one
        if ((!pEntity->P(OP_IsMoving) &&
            !m_regroupArea.IsInside(pEntity->Position())) ||
            (pEntity->P(OP_IsMoving) &&
            pEntity->TargetPosition() != m_formation.Placement.at(entityR.first)))
        {
            entityR.second->Entity()->Move(m_formation.Placement.at(entityR.first));
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void ArriveArmyState::Enter()
{
    ArmyState::Enter();

    m_arriveArea = Circle2(m_targetPos1, ArmyController::FocusAreaRadius);
}
//////////////////////////////////////////////////////////////////////////
void ArriveArmyState::DebugDraw()
{
    g_Game->DebugDrawMapCircle(m_arriveArea.Center, m_arriveArea.Radius, GCLR_Purple);
}
//////////////////////////////////////////////////////////////////////////
void ArriveArmyState::Update()
{
    ArmyState::Update();

    for (auto& entityR : m_controlledEntities)
    {
        // Re issue move to any idle out of order entity
        if (entityR.second->Entity()->P(OP_State) == OBJSTATE_Idle &&
            !m_arriveArea.IsInside(entityR.second->Entity()->Position()))
        {
            entityR.second->Entity()->Move(m_arriveArea.RandomInside());
        }
    }
}
//////////////////////////////////////////////////////////////////////////
//
// Machines
//
void StandArmyFSM::CheckTransitions()
{
    auto pController = (ArmyController*)m_pController;
    auto pCurrState = static_pointer_cast<ArmyState>(CurrentState());

    (void)pController;

    switch (pCurrState->TypeId())
    {
    case IdleArmyState::TypeID:
        if (!pController->IsInOrder(pCurrState->Entities(), pCurrState->TargetPosition()))
        {
            PushState(RegroupArmyState::TypeID);
        }
        break;
    case RegroupArmyState::TypeID:
        if (pController->IsInOrder(pCurrState->Entities(), pCurrState->TargetPosition()))
        {
            PopState();
        }
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
void GuardArmyFSM::CheckTransitions()
{
    auto pController = (ArmyController*)m_pController;
    auto pCurrState = static_pointer_cast<ArmyState>(CurrentState());

    switch (pCurrState->TypeId())
    {
    case AlarmArmyState::TypeID:
        if (pController->IsAnyEnemyInFormationAreaSight(pCurrState->TargetPosition()))
        {
            PushState(AttackArmyState::TypeID);
        }
        else if (!pController->IsInOrder(pCurrState->Entities(), pCurrState->TargetPosition()))
        {
            PushState(RegroupArmyState::TypeID);
        }

        break;
    case AttackArmyState::TypeID:
        if (!pController->IsAnyEnemyInFormationAreaSight(pCurrState->TargetPosition()))
        {
            PopState();
        }
        break;
    case RegroupArmyState::TypeID:
        if (pController->IsInOrder(pCurrState->Entities(), pCurrState->TargetPosition()))
        {
            PopState();
        }
        break;
    }
}
//////////////////////////////////////////////////////////////////////////
void AttackMoveArmyFSM::CheckTransitions()
{
    auto pController = (ArmyController*)m_pController;
    auto pCurrState = static_pointer_cast<ArmyState>(CurrentState());

    switch (pCurrState->TypeId())
    {
    case AlarmArmyState::TypeID:
        if (pController->TargetEntity() != INVALID_TID)
        {
            PushState(RegroupArmyState::TypeID);
            PushState(AttackArmyState::TypeID);
        }
        else if (!pController->IsInOrder(pCurrState->Entities(), pCurrState->TargetPosition()))
        {
            PushState(ArriveArmyState::TypeID);
        }
        break;
    case AttackArmyState::TypeID:
        if (pController->TargetEntity() == INVALID_TID)
        {
            PopState();
        }
        break;
    case ArriveArmyState::TypeID:
        if (pController->TargetEntity() != INVALID_TID)
        {
            PushState(AttackArmyState::TypeID);
            PushState(RegroupArmyState::TypeID);
        }
        else if (pController->IsInOrder(pCurrState->Entities(), pCurrState->TargetPosition()))
        {
            PopState();
            PushState(AlarmArmyState::TypeID);
        }
        break;
    case RegroupArmyState::TypeID:
        if (pController->IsInOrder(pCurrState->Entities(), pCurrState->TargetPosition()))
        {
            PopState();
        }
        break;
    }
}