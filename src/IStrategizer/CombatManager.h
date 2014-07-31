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
            m_reinforcementsArmy("ReinforcementsArmy", pConsultant),
            m_frontLinesArmy("FrontLinesArmy", pConsultant),
            m_brokenArmy("BrokenArmy", pConsultant)
        {}

        void Init();
        void Update();
        void AttackArea(_In_ Vector2 pos);
        void DefendArea(_In_ Vector2 pos);

        bool NeedReinforcements() { return true; }

    private:
        StrategySelectorPtr m_pConsultant;
        ArmyController m_frontLinesArmy;
        ArmyController m_reinforcementsArmy;
        ArmyController m_brokenArmy;
    };
}

#endif