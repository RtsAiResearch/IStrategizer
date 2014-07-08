#include "Battle.h"
#include "TargetFSMState.h"
#include "AttackFSMState.h"
#include "FinishedFSMState.h"
#include "MessagePump.h"
#include "DataMessage.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameType.h"
#include "GameEntity.h"

using namespace IStrategizer;
using namespace std;

Battle::Battle(RtsGame& game) :
    m_stateMachine(Target, Finished),
    m_currentTarget(DONT_CARE),
    m_nextTarget(DONT_CARE)
{
    SelectArmy(game);

    // Initialize the FSM
    m_stateMachine.AddState(new TargetFSMState<Battle*>(this));
    m_stateMachine.AddState(new AttackFSMState<Battle*>(this));
    m_stateMachine.AddState(new FinishedFSMState<Battle*>(this));

    // Register for messages
    g_MessagePump.RegisterForMessage(MSG_EntityDestroy, this);
}
//////////////////////////////////////////////////////////////////////////
void Battle::SelectArmy(RtsGame &game)
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
void Battle::NotifyMessegeSent(Message* p_msg)
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
                m_destroyedArmyEntities.push_back(pMsg->Data()->EntityId);
            }
        }
        else if (pMsg->Data()->OwnerId == PLAYER_Enemy)
        {
            if (m_nextTarget == pMsg->Data()->EntityId)
            {
                m_destroyedEnemyEntities.push_back(pMsg->Data()->EntityId);
            }
        }
    }
}
//////////////////////////////////////////////////////////////////////////
Battle::~Battle()
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