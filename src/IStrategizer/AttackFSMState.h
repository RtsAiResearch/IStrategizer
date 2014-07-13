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
        AttackFSMState(TController controller) : FSMState(Attack, controller) {}
        
        void Enter(RtsGame& game, const WorldClock& clock)
        {
            ControllerTraits<TController>::Type battle = m_controller;
            EntitySet army = TControllerTraits::Army(battle);
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
            EntitySet army = TControllerTraits::Army(battle);
            bool armyEmpty = army.empty();

            return armyEmpty ? Finished : Target;
        }
    };
}

#endif // ATTACKFSMSTATE_H