#ifndef ATTACKMANAGER_H
#define ATTACKMANAGER_H

#include "EngineComponent.h"
#include "EngineData.h"
#include "Battle.h"
#include "MessagePump.h"
#include "RtsGame.h"
#include <set>

namespace IStrategizer
{
    class WorldClock;

    class AttackManager : public EngineComponent
    {
    public:
        AttackManager() : EngineComponent("AttackManager") {}

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
                    g_MessagePump.Send(new Message(0, MSG_BattleComplete));
                }
            }

            for (auto battle : inactiveBattles)
            {
                m_battles.erase(battle);
                delete battle;
            }
        }

        void AddBattle() { m_battles.insert(new Battle(*g_Game)); }
        bool Active() const { return !m_battles.empty(); }

    private:
        std::set<Battle*> m_battles;
    };
}

#endif