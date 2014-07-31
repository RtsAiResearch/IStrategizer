#include "ArmyFSM.h"
#include "ArmyController.h"
#include "EntityFSM.h"
#include "GameEntity.h"

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
    m_controlledEntities = pController->HealthyEntities();
    m_targetPos1 = pController->TargetPosition();
    m_targetEntity = pController->TargetEntity();

    LogInfo("%s with %d controlled entities", ToString().c_str(), m_controlledEntities.size());
}
//////////////////////////////////////////////////////////////////////////
void ArmyState::Exit()
{
    LogInfo("%s with %d controlled entities", ToString().c_str(), m_controlledEntities.size());
    LogInfo("%s -> Exit", ToString().c_str());
}
//////////////////////////////////////////////////////////////////////////
void ArmyState::Update()
{
    auto pController = (ArmyController*)m_pController;

    if (!pController->TargetPosition().IsInf())
        g_Game->DebugDrawMapLine(pController->Center(), pController->TargetPosition(), GCLR_Orange);

    g_Game->DebugDrawMapCircle(pController->Center(), 32, GCLR_Orange);

    // <army-fsm-str><fsm-state-str><num-controlled>/<num-died>
    char stats[64];
    auto statsPos = pController->Center();

    sprintf_s(stats, "%s%s C:%d/D:%d",
        pController->Logic()->ToString().c_str(),
        ToString().c_str(),
        pController->HealthyEntities().size(),
        pController->TotalDiedEntities());
    g_Game->DebugDrawMapText(statsPos, stats);
    
    sprintf_s(stats, "HP:%d GA:%d", pController->TotalMaxHP(), pController->TotalGroundAttack());
    statsPos.Y += 10;
    g_Game->DebugDrawMapText(statsPos, stats);

    auto focusArea = pController->FormationArea();
    g_Game->DebugDrawMapCircle(focusArea.Center, focusArea.Radius, GCLR_Blue);

    auto sightArea = pController->SightArea();
    g_Game->DebugDrawMapCircle(sightArea.Center, sightArea.Radius, GCLR_Cyan);

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
void AttackArmyState::Enter()
{
    ArmyState::Enter();

    for (auto& entityR : m_controlledEntities)
    {
        entityR.second->PushLogic(StackFSMPtr(new HintNRunEntityFSM(&*entityR.second)));
    }

    LogInfo("Assigned attack logic for controlled entities");
}
//////////////////////////////////////////////////////////////////////////
void AttackArmyState::Exit()
{
    for (auto& entityR : m_controlledEntities)
    {
        entityR.second->PopLogic();
    }

    ArmyState::Exit();
}
//////////////////////////////////////////////////////////////////////////
void AlarmArmyState::Enter()
{
    ArmyState::Enter();
}
//////////////////////////////////////////////////////////////////////////
void AlarmArmyState::Exit()
{
    ArmyState::Exit();
}
//////////////////////////////////////////////////////////////////////////
void RegroupArmyState::Enter()
{
    ArmyState::Enter();

    m_formation.Reset();

    auto pController = (ArmyController*)m_pController;
    m_formation.Center = TargetPosition();
    pController->CalcGroupFormation(m_formation);
    
    m_regroupArea = Circle2(TargetPosition(), pController->FormationData().CircleRadius);

    for (auto& entityR : m_controlledEntities)
    {
        entityR.second->PushIdleLogic();
    }
}
//////////////////////////////////////////////////////////////////////////
void RegroupArmyState::Exit()
{
    for (auto& entityR : m_controlledEntities)
    {
        if (entityR.second->EntityExists())
            entityR.second->PopLogic();
    }

    m_regroupArea = Circle2(Vector2::Inf(), INT_MAX);

    ArmyState::Exit();
}
//////////////////////////////////////////////////////////////////////////
void RegroupArmyState::Update()
{
    ArmyState::Update();

    g_Game->DebugDrawMapCircle(m_regroupArea.Center, m_regroupArea.Radius, GCLR_Purple);

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

    for (auto& entityR : m_controlledEntities)
    {
        entityR.second->PushIdleLogic();
    }
}
//////////////////////////////////////////////////////////////////////////
void ArriveArmyState::Exit()
{
    for (auto& entityR : m_controlledEntities)
    {
        if (entityR.second->EntityExists())
            entityR.second->PopLogic();
    }

    ArmyState::Exit();
}
//////////////////////////////////////////////////////////////////////////
void ArriveArmyState::Update()
{
    ArmyState::Update();

    g_Game->DebugDrawMapCircle(m_arriveArea.Center, m_arriveArea.Radius, GCLR_Purple);

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
void GuardArmyFSM::CheckTransitions()
{
    auto pController = (ArmyController*)m_pController;
    auto pCurrState = static_pointer_cast<ArmyState>(CurrentState());

    switch (pCurrState->TypeId())
    {
    case AlarmArmyState::TypeID:
        if (pController->IsAnyEnemyTargetInSight())
        {
            PushState(AttackArmyState::TypeID);
        }
        else if (!pController->IsInOrder(pCurrState->Entities(), pCurrState->TargetPosition()))
        {
            PushState(RegroupArmyState::TypeID);
        }

        break;
    case AttackArmyState::TypeID:
        if (!pController->IsAnyEnemyTargetInSight())
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
        }
        else if (pController->IsInOrder(pCurrState->Entities(), pCurrState->TargetPosition()))
        {
            PopState();
        }
        break;
    }
}