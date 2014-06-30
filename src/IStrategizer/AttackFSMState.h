#ifndef ATTACKFSMSTATE_H
#define ATTACKFSMSTATE_H

#include "FSMState.h"
#include "EngineData.h"
#include "RtsGame.h"
#include "GamePlayer.h"
#include "GameType.h"
#include "GameEntity.h"
#include "EngineAssist.h"
#include "MessagePump.h"

namespace IStrategizer
{
    template<class TController, class TControllerTraits = ControllerTraits<TController>>
    class AttackFSMState : public FSMState<TController, ControllerTraits<TController>>
    {
    public:
        AttackFSMState(TController controller) : 
            m_targetEnemyEntity(DONT_CARE),
            m_targetDestroyed(false),
            FSMState(Attack, controller)
        { g_MessagePump.RegisterForMessage(MSG_EntityDestroy, this); }
        
        void Enter(RtsGame& game, const WorldClock& clock)
        {
            m_targetDestroyed = false;
            
            ControllerTraits<TController>::ConstType battle = m_controller;
            m_targetEnemyEntity = TControllerTraits::TargetEnemyEntity(battle);
        }

        void Update(RtsGame& game, const WorldClock& clock)
        {
            ControllerTraits<TController>::ConstType battle = m_controller;
            std::set<TID> army = TControllerTraits::Army(battle);
            TID targetId = DONT_CARE;

            for (TID attackerId : army)
            {
                GameEntity* pEntity = game.Self()->GetEntity(attackerId);
                targetId = pEntity->GetTargetId();
                if (targetId != DONT_CARE)
                {
                    // This piece of code is to verify that the target is common between
                    // all the attacking entities
                    //_ASSERTE(m_targetEnemyEntity == targetId);
                }
            }
        }

        int CheckTransitions(RtsGame& game, const WorldClock& clock)
        {
            ControllerTraits<TController>::ConstType battle = m_controller;
            std::set<TID> army = TControllerTraits::Army(battle);
            bool armyEmpty = army.empty();

            return armyEmpty ? Finished : m_targetDestroyed ? Deploy : Attack;
        }

        //////////////////////////////////////////////////////////////////////////
        void NotifyMessegeSent(Message* p_msg)
        {
            if (p_msg->MessageTypeID() == MSG_EntityDestroy)
            {
                EntityDestroyMessage* pMsg = static_cast<EntityDestroyMessage*>(p_msg);
                _ASSERTE(pMsg && pMsg->Data());

                if (pMsg->Data()->OwnerId == PLAYER_Enemy)
                {
                    if (m_targetEnemyEntity == pMsg->Data()->EntityId)
                    {
                        m_targetDestroyed = true;
                    }
                }
            }
        }

    private:
        bool m_targetDestroyed;
        TID m_targetEnemyEntity;
    };
}

#endif // ATTACKFSMSTATE_H