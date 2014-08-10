#include "ArmyFSM.h"
#include "ArmyController.h"
#include "EntityFSM.h"
#include "GameEntity.h"
#include "WorldMap.h"
#include "IStrategizerEx.h"
#include "RtsGame.h"
#include "GamePlayer.h"

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
    m_targetPos1 = pController->TargetPosition();
    m_targetEntity = pController->TargetEntity();
    m_lastControlled.clear();
    LogInfo("%s with %d controlled entities", ToString().c_str(), m_lastControlled.size());
}
//////////////////////////////////////////////////////////////////////////
void ArmyState::Exit()
{
    auto pController = (ArmyController*)m_pController;
    for (auto& entityR : pController->Entities())
    {
        if (m_lastControlled.count(entityR.first))
            entityR.second->PopLogic();
    }

    LogInfo("%s with %d controlled entities", ToString().c_str(), m_lastControlled.size());
    LogInfo("%s -> Exit", ToString().c_str());
}
//////////////////////////////////////////////////////////////////////////
void ArmyState::Update()
{
    m_newControlledCurrFrame = false;
    auto pController = (ArmyController*)m_pController;

    // Exclude no-controlled army entities
    for (auto itr = m_lastControlled.begin(); itr != m_lastControlled.end();)
    {
        if (pController->Entities().count(*itr) == 0)
        {
            auto pLostEntity = g_Game->Self()->GetEntity(*itr);
            LogInfo("Entity %s is not army controlled anymore, won't track it", (pLostEntity != nullptr ? pLostEntity->ToString().c_str() : to_string(*itr).c_str()));
            m_lastControlled.erase(itr++);
        }
        else
            ++itr;
    }

    // Include new controlled army entities
    for (auto& entityR : pController->Entities())
    {
        if (m_lastControlled.count(entityR.first) == 0)
        {
            LogInfo("Pushing proper logic to a new controlled entity %s", entityR.second->ToString().c_str());
            m_lastControlled.insert(entityR.first);
            auto pMicroLogic = g_Engine->StrategyMgr()->SelectMicroLogic(pController, &*entityR.second);
            entityR.second->PushLogic(pMicroLogic);

            if (!m_newControlledCurrFrame)
                m_newControlledCurrFrame = true;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void ArmyState::DebugDraw()
{
    auto pController = (ArmyController*)m_pController;

    if (!pController->TargetPosition().IsInf())
        g_Game->DebugDrawMapLine(pController->Center(), pController->TargetPosition(), GCLR_Orange);

    if (pController->TargetEntity() != INVALID_TID &&
        g_Game->GetEntity(pController->TargetEntity()) != nullptr)
    {
        auto pTargetPos = g_Game->GetEntity(pController->TargetEntity())->Position();

        // Draw a target cross symbol
        g_Game->DebugDrawMapCircle(pTargetPos, 3, GCLR_Red, true);
        g_Game->DebugDrawMapCircle(pTargetPos, 10, GCLR_Red, false);
        g_Game->DebugDrawMapLine(pTargetPos - Vector2(14, 0), pTargetPos + Vector2(14, 0), GCLR_Red);
        g_Game->DebugDrawMapLine(pTargetPos - Vector2(0, 14), pTargetPos + Vector2(0, 14), GCLR_Red);
    }

    g_Game->DebugDrawMapCircle(pController->Center(), 32, GCLR_Orange);
    g_Game->DebugDrawMapCircle(pController->Center(), 30, GCLR_Orange);

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
    g_Game->DebugDrawMapCircle(focusArea.Center, focusArea.Radius + 2, GCLR_Blue);

    auto sightArea = pController->SightArea();
    g_Game->DebugDrawMapCircle(sightArea.Center, sightArea.Radius, GCLR_Cyan);
}
//////////////////////////////////////////////////////////////////////////
void RegroupArmyState::Enter()
{
    ArmyState::Enter();
    m_formation.Reset();
}
//////////////////////////////////////////////////////////////////////////
void RegroupArmyState::Update()
{
    ArmyState::Update();

    auto pController = (ArmyController*)m_pController;

    if (m_newControlledCurrFrame)
    {
        if (m_regroupAtArmyCenter)
        {
            auto pClosestToCenter = pController->ClosestEntityToCenter();
            m_targetPos1 = g_Game->Map()->GetClosestReachableRegionCenter(pClosestToCenter->Id());
        }
        else
            m_targetPos1 = pController->TargetPosition();

        m_formation.Center = TargetPosition();
        pController->CalcGroupFormation(m_formation);
        m_regroupArea = Circle2(TargetPosition(), pController->FormationData().CircleRadius);
    }

    for (auto& entityR : pController->Entities())
    {
        auto pEntity = entityR.second->Entity();

        // Re issue move to any astray unit
        // 1- Outside the formation area
        // 2- Not moving and outside regroup area
        // 3- Moving but its target position is not its designated one
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
void RegroupArmyState::DebugDraw()
{
    ArmyState::DebugDraw();

    g_Game->DebugDrawMapCircle(m_regroupArea.Center, m_regroupArea.Radius, GCLR_Purple);

    for (auto& p : m_formation.Placement)
    {
        g_Game->DebugDrawMapRectangle(p.second, p.second + ArmyController::FormationSpacing, GCLR_Blue, false);
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
    ArmyState::DebugDraw();

    g_Game->DebugDrawMapCircle(m_arriveArea.Center, m_arriveArea.Radius, GCLR_Purple);
}
//////////////////////////////////////////////////////////////////////////
void ArriveArmyState::Update()
{
    ArmyState::Update();

    auto pController = (ArmyController*)m_pController;

    for (auto& entityR : pController->Entities())
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
        if (!pController->IsInOrder(pCurrState->TargetPosition()))
        {
            PushState(RegroupArmyState::TypeID);
        }
        break;
    case RegroupArmyState::TypeID:
        if (pController->IsInOrder(pCurrState->TargetPosition()))
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
        else if (!pController->IsInOrder(pCurrState->TargetPosition()))
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
        if (pController->IsInOrder(pCurrState->TargetPosition()))
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
            PushState(AttackArmyState::TypeID);
        }
        else if (!pController->IsInOrder(pCurrState->TargetPosition()))
        {
            PushState(ArriveArmyState::TypeID);
        }
        break;
    case AttackArmyState::TypeID:
        if (pController->TargetEntity() == INVALID_TID)
        {
            PopState();
        }
        else if (!pController->IsInOrder(pController->ClosestEntityToCenter()->Position(), 0.60f))
        {
            PushState(RegroupArmyState::TypeID);
        }
        break;
    case ArriveArmyState::TypeID:
        if (pController->TargetEntity() != INVALID_TID)
        {
            PushState(AttackArmyState::TypeID);
        }
        else if (pController->IsInOrder(pCurrState->TargetPosition()))
        {
            PopState();
            PushState(AlarmArmyState::TypeID);
        }
        break;
    case RegroupArmyState::TypeID:
        if (pController->IsInOrder(pCurrState->TargetPosition(), 0.80f))
        {
            PopState();
        }
        break;
    }
}