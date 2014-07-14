#ifndef ATTACKMANAGER_H
#define ATTACKMANAGER_H

#include "EngineObject.h"
#include "EngineData.h"
#include "Battle.h"
#include "MessagePump.h"
#include "RtsGame.h"
#include "StrategySelector.h"
#include "Army.h"
#include "FSMState.h"
#include <set>
#include <vector>

namespace IStrategizer
{
    class WorldClock;

    class AttackManager : public EngineObject
    {
    public:
        AttackManager(StrategySelector* pSelector) : m_pStrategySelector(pSelector) {}

        ~AttackManager()
        {
            for (Battle* battle : m_battles)
            {
                delete battle;
            }

            m_battles.clear();
        }

        void Update(RtsGame& game, const WorldClock& clock)
        {
            if (m_battles.empty())
                return;

            std::set<Battle*> inactiveBattles;

            for (auto battle : m_battles)
            {
                if (battle->Active())
                {
                    battle->Update(game, clock);
                }
                else
                {
                    inactiveBattles.insert(battle);
                    g_MessagePump->Send(new Message(0, MSG_BattleComplete));
                }
            }

            for (auto battle : inactiveBattles)
            {
                m_battles.erase(battle);
                delete battle;
            }
        }

        void AddBattle()
        {
            std::vector<FSMState<Battle*>*> states;

            Army* pArmy = new Army(*g_Game);
            m_pStrategySelector->SelectAttackStrategy(pArmy, states);
            m_battles.insert(new Battle(pArmy, states));
        }

        bool Active() const { return !m_battles.empty(); }

    private:
        std::set<Battle*> m_battles;
        StrategySelector* m_pStrategySelector;
    };
}

#endif