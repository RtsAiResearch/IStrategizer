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

Army::Army(RtsGame& game)
{
    SelectArmy(game);
    g_MessagePump->RegisterForMessage(MSG_EntityDestroy, this);
}
//////////////////////////////////////////////////////////////////////////
void Army::SelectArmy(RtsGame &game)
{
    EntityList entities;
    game.Self()->Entities(entities);
    for (TID entityId : entities)
    {
        GameEntity* pAttacker = game.Self()->GetEntity(entityId);
        GameType* pEntityType = game.GetEntityType(pAttacker->Type());

        if (pEntityType->Attr(ECATTR_IsAttacker) &&
            g_Assist.IsEntityObjectReady(entityId) &&
            !pAttacker->IsLocked())
        {
            m_army.insert(entityId);
            pAttacker->Lock(this);
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
            if (m_army.count(pMsg->Data()->EntityId) != 0)
            {
                m_army.erase(pMsg->Data()->EntityId);
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
Army::~Army()
{
    if (g_Game != nullptr)
    {
        for (auto attackerId : m_army)
        {
            GameEntity* pAttacker = g_Game->Self()->GetEntity(attackerId);
            if (pAttacker != nullptr)
            {
                pAttacker->Unlock(this);
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
Vector2 Army::Center() const
{
    vector<Vector2> positions;
    Vector2 upperLeft, lowerRight;

    for (TID entityId : m_army)
    {
        GameEntity* pAttacker = g_Game->Self()->GetEntity(entityId);
        positions.push_back(pAttacker->GetPosition());
    }

    MathHelper::MinimumBoundingBox(positions, upperLeft, lowerRight);

    int centerX = (upperLeft.X + lowerRight.X) / 2;
    int centerY = (upperLeft.Y + lowerRight.Y) / 2;
    return Vector2(centerX, centerY);
}