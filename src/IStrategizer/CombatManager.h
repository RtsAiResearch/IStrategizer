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

        void AttackArea(_In_ Vector2 pos)
        {
            _ASSERTE(!pos.IsInf());
            m_armyCtrlr.ControlArmy();
            m_armyCtrlr.AttackArea(pos);
        }

        void DefendArea(_In_ Vector2 pos)
        {
            _ASSERTE(!pos.IsInf());
            m_armyCtrlr.ControlArmy();
            m_armyCtrlr.DefendArea(pos);
        }

        bool NeedReinforcements() { return true; }

    private:
        StrategySelectorPtr m_pConsultant;
        ArmyController m_armyCtrlr;
    };
}

#endif