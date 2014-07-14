#ifndef TANKATTACKFSMSTATE
#define TANKATTACKFSMSTATE

#include "FSMState.h"
#include "EngineData.h"
#include "RtsGame.h"
#include "BWAPI.h"

using namespace BWAPI;

namespace IStrategizer
{
    template<class TController, class TControllerTraits = ControllerTraits<TController>>
    class TankAttackFSMState : public FSMState<TController, ControllerTraits<TController>>
    {
    public:
        TankAttackFSMState(TController controller) : FSMState(TankAttack, controller) {}

        void Enter(RtsGame& game, const WorldClock& clock)
        {
            ControllerTraits<TController>::ConstType battle = m_controller;
            Army* pArmy = TControllerTraits::GetArmy(battle);

            for (TID attackerId : pArmy->Entities())
            {
                Unit pAttacker = Broodwar->getUnit(attackerId);
                
                if (pAttacker->getType().getID() == UnitTypes::Terran_Siege_Tank_Tank_Mode.getID())
                {
                    _ASSERT(pAttacker->canSiege());
                    pAttacker->siege();
                }
            }
        }

        void Update(RtsGame& game, const WorldClock& clock)
        {
            
        }

        int CheckTransitions(RtsGame& game, const WorldClock& clock)
        {
            return TankAttack;
        }
    };
}

#endif // TANKATTACKFSMSTATE
