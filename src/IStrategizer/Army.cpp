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

Army::Army() :
m_armySize(0)
{
    g_MessagePump->RegisterForMessage(MSG_EntityDestroy, this);
}
//////////////////////////////////////////////////////////////////////////
void Army::SelectArmy(_In_ RtsGame &game)
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
void Army::NotifyMessegeSent(_In_ Message* pMsg)
{
    if (pMsg->TypeId() == MSG_EntityDestroy)
    {
        EntityDestroyMessage* pDestroyMsg = static_cast<EntityDestroyMessage*>(pMsg);
        _ASSERTE(pDestroyMsg && pDestroyMsg->Data());

        if (pDestroyMsg->Data()->OwnerId == PLAYER_Self)
        {
            EntityClassType type = pDestroyMsg->Data()->EntityType;
            ArmyGroup group = Classify(g_Game->GetEntityType(type));

            auto& groupSet = m_groups[group];
            if (groupSet.erase(pDestroyMsg->Data()->EntityId))
            {
                LogInfo("Removed entity %s[%d] from army", Enums[type], pDestroyMsg->Data()->EntityId);
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
        Release();
    }
}
//////////////////////////////////////////////////////////////////////////
void Army::Release()
{
    for (auto& group : m_groups)
    {
        for (auto entityId : group.second)
        {
            GameEntity* pAttacker = g_Game->Self()->GetEntity(entityId);

            if (pAttacker != nullptr && pAttacker->IsLocked())
                pAttacker->Unlock(this);
        }
    }
}
//////////////////////////////////////////////////////////////////////////
Vector2 Army::Center() const
{
    Vector2 center;

    for (auto& group : m_groups)
    {
        for (auto entityId : group.second)
        {
            auto pEntity = g_Game->Self()->GetEntity(entityId);
            center += pEntity->GetPosition();
        }
    }

    center /= m_armySize;

    return center;
}
//////////////////////////////////////////////////////////////////////////
ArmyGroup Army::Classify(const GameType* pType)
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
void Army::Exclude(_In_ TID entityId)
{
    auto pEntity = g_Game->Self()->GetEntity(entityId);

    auto group = Classify(pEntity->Type());

    auto& groupSet = m_groups.at(group);

    if (groupSet.erase(entityId))
    {
        LogDebugInfo("Excluded %s from army", pEntity->ToString().c_str());
    }
}