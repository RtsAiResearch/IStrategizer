#ifndef ATTACKMANAGER_H
#define ATTACKMANAGER_H

#include "ArmyController.h"
#include "MessagePump.h"
#include "RtsGame.h"
#include "StrategyManager.h"
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
        static const int FrontlinesArmyBaseDistance = 400;
        static const int BrokenArmyBaseDistance = 200;

        CombatManager() :
            m_frontLinesArmy("FrontLinesArmy"),
            m_brokenArmy("BrokenArmy"),
            m_currOrder(CMBTMGR_Defend),
            m_armiesBaseLoc(Vector2::Inf())
        {}

        void Init();
        void Update();
        void AttackEnemy(_In_ Vector2 pos);
        void DefendBase();
        void DebugDraw();
        CombatManagerOrder CurrentOrder() const { return m_currOrder; }

    private:
        Vector2 m_armiesBaseLoc;
        StrategyManagerPtr m_pConsultant;
        ArmyController m_frontLinesArmy;
        ArmyController m_brokenArmy;
        CombatManagerOrder m_currOrder;
    };
}

#endif