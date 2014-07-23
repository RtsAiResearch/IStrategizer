#ifndef BATTLE_H
#define BATTLE_H

#include "Army.h"
#include "StackFSM.h"
#include "StrategySelector.h"

namespace IStrategizer
{
    class ArmyController : public EngineObject
    {
    public:
        ArmyController(StrategySelectorPtr pConsultant) :
            m_pConsultant(pConsultant),
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
        void AttackArea(_In_ Circle2 area) {}
        void DefendArea(_In_ Circle2 area) {}

    private:
        StackFSMPtr m_pMachine;
        Army m_army;
        TID m_currentTarget;
        StrategySelectorPtr m_pConsultant;
    };
}

#endif // BATTLE_H