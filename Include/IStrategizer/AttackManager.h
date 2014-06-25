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
        AttackManager() : m_pCurrentBattle(nullptr), EngineComponent("AttackManager")
        {
            g_MessagePump.RegisterForMessage(MSG_PlanComplete, this);
        }

        void Update(RtsGame& game, const WorldClock& clock)
        {
            if (m_pCurrentBattle == nullptr)
                return;

            m_pCurrentBattle->Update(game, clock);

            if (!m_pCurrentBattle->Active())
            {
                m_pCurrentBattle = nullptr;
            }
        }

        bool Active() const { return m_pCurrentBattle != nullptr; }

        void NotifyMessegeSent(Message* p_message)
        {
            if (m_pCurrentBattle == nullptr && 
                p_message->MessageTypeID() == MSG_PlanComplete)
            {
                m_pCurrentBattle = new Battle(*g_Game);
            }
        }

    private:
        std::vector<Battle*> m_battlesHistory;
        Battle* m_pCurrentBattle;
    };
}
#endif