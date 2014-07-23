#ifndef BATTLE_H
#define BATTLE_H

#include "Army.h"
#include "StackFSM.h"

namespace IStrategizer
{
    class ArmyController : public EngineObject
    {
    public:
        ArmyController() :
            m_currentTarget(INVALID_TID)
        {
        }

        void Update(_In_ RtsGame& game)
        {
            if (m_pMachine != nullptr)
                m_pMachine->Update(game); 
        }

        TID CurrentTarget() const { return m_currentTarget; }
        const Army& GetArmy() const { return m_army; }
        void AttackArea(_In_ Circle2 area);
        void DefendArea(_In_ Circle2 area);

    private:
        StackFSMPtr m_pMachine;
        Army m_army;
        TID m_currentTarget;
    };
}

#endif // BATTLE_H