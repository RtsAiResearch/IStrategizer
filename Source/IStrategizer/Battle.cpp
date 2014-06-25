#include "Battle.h"
#include "DeployFSMState.h"
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
    EngineComponent("Battle"),
    m_stateMachine(Deploy, Finished)
{
    SelectArmy(game);

    // Initialize the FSM
    m_stateMachine.AddState(new DeployFSMState<Battle*>(this));
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
        GameType* pEntityType = game.GetEntityType(game.Self()->GetEntity(entityId)->Type());

        if (!pEntityType->Attr(ECATTR_IsCowrad) && pEntityType->Attr(ECATTR_CanAttack))
        {
            m_army.insert(entityId);
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
            if (m_targetEnemyEntity == pMsg->Data()->EntityId)
            {
                m_destroyedEnemyEntities.push_back(pMsg->Data()->EntityId);
            }
        }
    }
}