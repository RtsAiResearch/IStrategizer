#ifndef ATTACKMANAGER_H
#define ATTACKMANAGER_H

#include "EngineComponent.h"
#include "EngineData.h"
#include "Battle.h"
#include "MessagePump.h"
#include "RtsGame.h"
#include <vector>

namespace IStrategizer
{
    class WorldClock;

    class AttackManager : public EngineComponent
    {
    public:
        AttackManager() : m_pCurrentBattle(nullptr), EngineComponent("AttackManager") {}

        void Update(RtsGame& game, const WorldClock& clock)
        {
            if (m_pCurrentBattle == nullptr)
                return;

            m_pCurrentBattle->Update(game, clock);

            if (!m_pCurrentBattle->Active())
            {
                g_MessagePump.Send(new Message(0, MSG_AttackComplete));
                m_pCurrentBattle = nullptr;
            }
        }

        void StartBattle() { m_pCurrentBattle = new Battle(*g_Game); }

        bool Active() const { return m_pCurrentBattle != nullptr; }

    private:
        std::vector<Battle*> m_battlesHistory;
        Battle* m_pCurrentBattle;
    };
}

#endif