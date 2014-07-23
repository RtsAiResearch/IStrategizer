#ifndef ATTACKMANAGER_H
#define ATTACKMANAGER_H

#include "ArmyController.h"
#include "MessagePump.h"
#include "RtsGame.h"
#include "StrategySelector.h"
#include <set>
#include <vector>

namespace IStrategizer
{
    class WorldClock;

    class CombatManager : public EngineObject
    {
    public:
        CombatManager(StrategySelectorPtr pSelector) : m_pStrategySelector(pSelector) {}

        void Update(_In_ RtsGame& game)
        {
            m_armyCtrlr.Update(game);
        }

        void AttackArea(_In_ Circle2 area)
        {

        }

        void DefendArea(_In_ Circle2 area)
        {
            // For now, defend is just an army attack, 
            // until base buildings and workers are part of the equation
            AttackArea(area);
        }

    private:
        StrategySelectorPtr m_pStrategySelector;
        ArmyController m_armyCtrlr;
    };
}

#endif