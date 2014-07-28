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
void ArmyState::Reset()
{
    m_targetPos1 = Vector2::Inf();
    m_targetPos1 = Vector2::Inf();
    m_targetEntity = INVALID_TID;
    m_controlledEntities.clear();

    LogInfo("%s -> Reset", ToString().c_str());

}
//////////////////////////////////////////////////////////////////////////
void ArmyState::Enter()
{
    LogInfo("%s -> Enter", ToString().c_str());

    Reset();

    auto pController = (ArmyController*)m_pController;
    m_controlledEntities = pController->HealthyEntities();
    m_targetPos1 = pController->TargetPosition();

    LogInfo("%s with %d controlled entities", ToString().c_str(), m_controlledEntities.size());
}
//////////////////////////////////////////////////////////////////////////
void ArmyState::Exit()
{
    LogInfo("%s with %d controlled entities", ToString().c_str(), m_controlledEntities.size());

    Reset();

    LogInfo("%s -> Exit", ToString().c_str());
}
//////////////////////////////////////////////////////////////////////////
void ArmyState::Update()
{
    auto pController = (ArmyController*)m_pController;

    if (!pController->TargetPosition().IsInf())
        g_Game->DebugDrawMapLine(pController->Center(), pController->TargetPosition(), GCLR_Orange);

    g_Game->DebugDrawMapCircle(pController->Center(), 32, GCLR_Orange);
    g_Game->DebugDrawMapText(pController->Center(), ToString().c_str());

    auto focusArea = pController->FocusArea();
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

    m_regroupArea = Circle2(m_targetPos1, ArmyController::FocusAreaRadius);

    for (auto& entityR : m_controlledEntities)
    {
        //entityR.second->PushLogic(StackFSMPtr(new GuardEntityFSM(&*entityR.second)));
        entityR.second->PushIdleLogic();
        entityR.second->Entity()->Move(m_regroupArea.RandomInside());
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

    ArmyState::Exit();
}
//////////////////////////////////////////////////////////////////////////
void RegroupArmyState::Update()
{
    ArmyState::Update();

    g_Game->DebugDrawMapCircle(m_regroupArea.Center, m_regroupArea.Radius, GCLR_Purple);

    for (auto& entityR : m_controlledEntities)
    {
        // Re issue move to any idle out of order entity
        if (entityR.second->Entity()->P(OP_State) == OBJSTATE_Idle &&
            !m_regroupArea.IsInside(entityR.second->Entity()->GetPosition()))
        {
            entityR.second->Entity()->Move(m_regroupArea.RandomInside());
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
        if (!pController->IsInOrder(pCurrState->Entities(), pCurrState->TargetPosition1()))
        {
            PushState(RegroupArmyState::TypeID);
        }
        else if (pController->IsAnyEnemyTargetInSight())
        {
            PushState(AttackArmyState::TypeID);
        }
        break;
    case AttackArmyState::TypeID:
        if (!pController->IsInOrder(pCurrState->Entities(), pCurrState->TargetPosition1()))
        {
            PushState(RegroupArmyState::TypeID);
        }
        if (!pController->IsAnyEnemyTargetInSight())
        {
            PopState();
        }
        break;
    case RegroupArmyState::TypeID:
        if (pController->IsInOrder(pCurrState->Entities(), pCurrState->TargetPosition1()))
        {
            PopState();
        }
        break;
    }
}

