#include "Army.h"
#include "MessagePump.h"
#include "DataMessage.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameType.h"
#include "GameEntity.h"
#include "EngineAssist.h"
#include "MathHelper.h"

using namespace IStrategizer;
using namespace std;

Army::Army(RtsGame& game) :
m_armySize(0)
{
    SelectArmy(game);
    g_MessagePump->RegisterForMessage(MSG_EntityDestroy, this);
}
//////////////////////////////////////////////////////////////////////////
void Army::SelectArmy(RtsGame &game)
{
    m_groups[ARMGRP_MeleelAttacker].clear();
    m_groups[ARMGRP_RangedAttacker].clear();
    m_groups[ARMGRP_Supporter].clear();

    m_armySize = 0;
    EntityList entities;
    game.Self()->Entities(entities);
    for (TID entityId : entities)
    {
        GameEntity* pAttacker = game.Self()->GetEntity(entityId);
        GameType* pEntityType = game.GetEntityType(pAttacker->TypeId());

        if (!pEntityType->Attr(ECATTR_IsWorker) &&
            !pEntityType->Attr(ECATTR_IsBuilding) &&
            g_Assist.IsEntityObjectReady(entityId) &&
            !pAttacker->IsLocked())
        {
            auto group = Classify(pEntityType);
            m_groups[group].insert(entityId);
            pAttacker->Lock(this);
            ++m_armySize;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void Army::NotifyMessegeSent(Message* p_msg)
{
    if (p_msg->MessageTypeID() == MSG_EntityDestroy)
    {
        EntityDestroyMessage* pMsg = static_cast<EntityDestroyMessage*>(p_msg);
        _ASSERTE(pMsg && pMsg->Data());

        if (pMsg->Data()->OwnerId == PLAYER_Self)
        {
            EntityClassType type = pMsg->Data()->EntityType;
            ArmyGroup group = Classify(g_Game->GetEntityType(type));

            auto& groupSet = m_groups[group];
            if (groupSet.erase(pMsg->Data()->EntityId))
            {
                LogInfo("Removed entity %s[%d] from army", Enums[type], pMsg->Data()->EntityId);
                _ASSERTE(m_armySize > 0);
                --m_armySize;
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
Army::~Army()
{
    if (g_Game != nullptr)
    {
        for (auto& group : m_groups)
        {
            for (auto entityId : group.second)
            {
                GameEntity* pAttacker = g_Game->Self()->GetEntity(entityId);
                if (pAttacker != nullptr)
                {
                    pAttacker->Unlock(this);
                }
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
Vector2 Army::Center() const
{
    Vector2 center;
    int armySize = 0;

    for (auto& group : m_groups)
    {
        for (auto entityId : group.second)
        {
            auto pEntity = g_Game->Self()->GetEntity(entityId);
            center += pEntity->GetPosition();
            ++armySize;
        }
    }

    center /= armySize;

    return center;
}
//////////////////////////////////////////////////////////////////////////
TID Army::Leader() const
{
    if (Empty())
        return INVALID_TID;

    if (m_groups.count(ARMGRP_MeleelAttacker) > 0 &&
        !m_groups.at(ARMGRP_MeleelAttacker).empty())
        return *m_groups.at(ARMGRP_MeleelAttacker).begin();
    else if (m_groups.count(ARMGRP_RangedAttacker) > 0 &&
        !m_groups.at(ARMGRP_RangedAttacker).empty())
        return *m_groups.at(ARMGRP_RangedAttacker).begin();
    else if (m_groups.count(ARMGRP_Supporter) > 0 &&
        !m_groups.at(ARMGRP_Supporter).empty())
        return *m_groups.at(ARMGRP_Supporter).begin();
    else return INVALID_TID;
}
//////////////////////////////////////////////////////////////////////////
ArmyGroup Army::Classify(const GameType* pType)
{
    if (!pType->Attr(ECATTR_IsAttacker))
    {
        return ARMGRP_Supporter;
    }
    else if (pType->Attr(ECATTR_IsMelee))
    {
        return ARMGRP_MeleelAttacker;
    }
    else
        return ARMGRP_RangedAttacker;
}
//////////////////////////////////////////////////////////////////////////
bool Army::HasType(EntityClassType type)
{
    auto group = Classify(g_Game->GetEntityType(type));

    auto& groupSet = m_groups.at(group);
    for (auto r : groupSet)
    {
        auto pEntity = g_Game->Self()->GetEntity(r);

        if (pEntity->TypeId() == type)
            return true;
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
void Army::Move(Vector2 pos, Vector2 formationFrontDir)
{
    for (auto& group : m_groups)
    {
        for (auto entityId : group.second)
        {
            auto pEntity = g_Game->Self()->GetEntity(entityId);
            pEntity->Move(pos);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void Army::Stop()
{
    for (auto& group : m_groups)
    {
        for (auto entityId : group.second)
        {
            auto pEntity = g_Game->Self()->GetEntity(entityId);
            pEntity->Stop();
        }
    }
}
//////////////////////////////////////////////////////////////////////////
void Army::UseTech(ResearchType tech)
{
}
//////////////////////////////////////////////////////////////////////////
void Army::Attack(TID targetId)
{
    for (auto& group : m_groups)
    {
        if (group.first == ARMGRP_Supporter)
            continue;;

        for (auto entityId : group.second)
        {
            auto pEntity = g_Game->Self()->GetEntity(entityId);
            pEntity->AttackEntity(targetId);
        }
    }

    // Supporters follow army leader
    if (!m_groups.at(ARMGRP_Supporter).empty())
    {
        TID armyLeaderId = Leader();
        _ASSERTE(armyLeaderId != INVALID_TID);

        for (auto entityId : m_groups.at(ARMGRP_Supporter))
        {
            auto pEntity = g_Game->Self()->GetEntity(entityId);
            pEntity->Follow(armyLeaderId);
        }
    }
}