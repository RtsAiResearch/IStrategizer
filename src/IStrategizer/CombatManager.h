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

    enum CombatManagerOrder
    {
        CMBTMGR_Attack,
        CMBTMGR_Defend
    };

    class CombatManager : public EngineObject
    {
    public:
        CombatManager(StrategySelectorPtr pConsultant) :
            m_pConsultant(pConsultant),
            m_reinforcementsArmy("ReinforcementsArmy", pConsultant),
            m_frontLinesArmy("FrontLinesArmy", pConsultant),
            m_brokenArmy("BrokenArmy", pConsultant),
            m_currOrder(CMBTMGR_Defend),
            m_armiesBaseLoc(Vector2::Inf())
        {}

        void Init();
        void Update();
        void AttackArea(_In_ Vector2 pos);
        void DefendArea(_In_ Vector2 pos);
        void DebugDraw();
        CombatManagerOrder CurrentOrder() const { return m_currOrder; }

    private:
        Vector2 m_armiesBaseLoc;
        StrategySelectorPtr m_pConsultant;
        ArmyController m_frontLinesArmy;
        ArmyController m_reinforcementsArmy;
        ArmyController m_brokenArmy;
        CombatManagerOrder m_currOrder;
    };
}

#endif