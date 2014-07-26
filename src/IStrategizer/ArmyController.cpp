#include "ArmyController.h"
#include "MessagePump.h"
#include "DataMessage.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameType.h"
#include "GameEntity.h"
#include "EngineAssist.h"
#include "MathHelper.h"
#include "EntityFSM.h"

using namespace IStrategizer;
using namespace std;

ArmyController::ArmyController(StrategySelectorPtr pConsultant) :
m_pConsultant(pConsultant),
m_currentTarget(INVALID_TID),
m_armySize(0)
{
    g_MessagePump->RegisterForMessage(MSG_EntityDestroy, this);
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::ControlArmy()
{
    ReleaseArmy();

    // For now, army controls only current free attackers
    for (auto entityR : g_Game->Self()->Entities())
    {
        auto pEntity = entityR.second;
        auto pEntityType = pEntity->Type();

        if (!pEntityType->Attr(ECATTR_IsWorker) &&
            !pEntityType->Attr(ECATTR_IsBuilding) &&
            g_Assist.IsEntityObjectReady(entityR.first) &&
            !pEntity->IsLocked())
        {
            auto group = Classify(pEntityType);
            auto pController = EntityControllerPtr(new EntityController(this));
            m_groups[group][entityR.first] = pController;
            pController->ControlEntity(entityR.first);
            pController->SetLogic(StackFSMPtr(new IdleEntityFSM(&*pController)));

            ++m_armySize;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::ReleaseArmy()
{
    m_armySize = 0;
    for (auto& group : m_groups)
    {
        for (auto& entityR : group.second)
        {
            entityR.second->ReleaseEntity();
        }
    }
    m_groups.clear();
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::NotifyMessegeSent(_In_ Message* pMsg)
{
    if (pMsg->TypeId() == MSG_EntityDestroy)
    {
        EntityDestroyMessage* pDestroyMsg = static_cast<EntityDestroyMessage*>(pMsg);
        _ASSERTE(pDestroyMsg && pDestroyMsg->Data());

        if (pDestroyMsg->Data()->OwnerId == PLAYER_Self)
        {
            ReleaseEntity(pDestroyMsg->Data()->EntityId);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
Vector2 ArmyController::Center() const
{
    Vector2 center;

    for (auto& group : m_groups)
    {
        for (auto entityId : group.second)
        {
            center += entityId.second->Entity()->GetPosition();
        }
    }

    center /= m_armySize;

    return center;
}
//////////////////////////////////////////////////////////////////////////
ArmyGroup ArmyController::Classify(const GameType* pType)
{
    if (pType->Attr(ECATTR_IsWorker))
        return ARMGRP_Worker;
    if (!pType->Attr(ECATTR_IsAttacker))
        return ARMGRP_Supporter;
    else if (pType->Attr(ECATTR_IsMelee))
        return ARMGRP_MeleelAttacker;
    else
        return ARMGRP_RangedAttacker;
}
//////////////////////////////////////////////////////////////////////////
bool ArmyController::HasType(EntityClassType type)
{
    auto group = Classify(g_Game->GetEntityType(type));

    if (m_groups.count(group) > 0)
    {
        auto& groupSet = m_groups.at(group);
        for (auto& entityR : groupSet)
        {
            if (entityR.second->Entity()->TypeId() == type)
                return true;
        }
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::ReleaseEntity(_In_ TID entityId)
{
    auto pEntity = g_Game->Self()->GetEntity(entityId);
    auto group = Classify(pEntity->Type());

    if (m_groups.count(group) > 0)
    {
        auto& groupSet = m_groups.at(group);

        if (groupSet.count(entityId) > 0)
        {
            groupSet.at(entityId)->ReleaseEntity();

            if (groupSet.erase(entityId))
            {
                LogInfo("Excluded %s from army", pEntity->ToString().c_str());
                --m_armySize;
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::Update()
{
    for (auto& group : m_groups)
    {
        for (auto& entityR : group.second)
        {
            auto pEntityCtrlr = entityR.second;
            pEntityCtrlr->Update();
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void ArmyController::DefendArea(_In_ Vector2 pos)
{
    for (auto& group : m_groups)
    {
        for (auto& entityR : group.second)
        {
            auto pEntityCtrlr = entityR.second;
            pEntityCtrlr->SetLogic(StackFSMPtr(new GuardEntityFSM(&*entityR.second)));
            pEntityCtrlr->TargetPosition(pos);
            pEntityCtrlr->ResetLogic();
        }
    }
}