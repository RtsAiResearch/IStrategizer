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
        CombatManager(StrategySelectorPtr pConsultant) :
            m_pConsultant(pConsultant),
            m_armyCtrlr(pConsultant)
        {}

        void Init() {}

        void Update()
        {
            m_armyCtrlr.Update();
        }

        void AttackArea(_In_ Circle2 area)
        {
            _ASSERTE(!area.Center.IsInf());
            m_armyCtrlr.AttackArea(area);
        }

        void DefendArea(_In_ Circle2 area)
        {
            _ASSERTE(!area.Center.IsInf());
            // For now, defend is just an army attack, 
            // until base buildings and workers are part of the equation
            AttackArea(area);
        }

        void ScoutLocation(_In_ Vector2 loc)
        {

        }

        bool NeedReinforcements() { return true; }

    private:
        StrategySelectorPtr m_pConsultant;
        ArmyController m_armyCtrlr;
    };
}

#endif