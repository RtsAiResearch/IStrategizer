#ifndef TANKATTACKFSMSTATE
#define TANKATTACKFSMSTATE

#include "FSMState.h"
#include "EngineData.h"
#include "RtsGame.h"
#include "DefinitionCrossMapping.h"
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

            pArmy->UseTech(g_Database.UpgradeMapping.GetByFirst(TechTypes::Tank_Siege_Mode.getID()));
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
