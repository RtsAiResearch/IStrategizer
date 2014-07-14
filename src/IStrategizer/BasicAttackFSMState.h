#ifndef BASICATTACKFSMSTATE_H
#define BASICATTACKFSMSTATE_H

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
    class BasicAttackFSMState : public FSMState<TController, ControllerTraits<TController>>
    {
    public:
        BasicAttackFSMState(TController controller) : FSMState(BasicAttack, controller) {}
        
        void Enter(RtsGame& game, const WorldClock& clock)
        {
            ControllerTraits<TController>::Type battle = m_controller;
            EntitySet army = TControllerTraits::GetArmy(battle)->Entities();
            TID nextTargetId = TControllerTraits::NextTarget(battle);
            TID currentTargetId = TControllerTraits::CurrentTarget(battle);

            if (currentTargetId != nextTargetId)
            {
                TControllerTraits::CurrentTarget(battle, nextTargetId);
                
                for (TID entityId : army)
                {
                    GameEntity* pEntity = game.Self()->GetEntity(entityId);
                    _ASSERTE(pEntity);

                    pEntity->AttackEntity(nextTargetId);
                }
            }
        }

        int CheckTransitions(RtsGame& game, const WorldClock& clock)
        {
            ControllerTraits<TController>::ConstType battle = m_controller;
            Army* pArmy = TControllerTraits::GetArmy(battle);

            return pArmy->Empty() ? Finished : Target;
        }
    };
}

#endif // BASICATTACKFSMSTATE_H
