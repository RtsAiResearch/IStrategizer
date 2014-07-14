#ifndef BATTLE_H
#define BATTLE_H

#include "EngineObject.h"
#include "Army.h"
#include "FSMMachine.h"

namespace IStrategizer
{
    class RtsGame;
    class WorldClock;
    template<class T>
    struct ControllerTraits<T*>;

    class Battle : public EngineObject
    {
    public:
        Battle::Battle(Army* pArmy, std::vector<FSMState<Battle*>*>& states) :
            m_stateMachine(states.front()->StateType(), states.back()->StateType()),
            m_currentTarget(DONT_CARE),
            m_nextTarget(DONT_CARE),
            m_pArmy(pArmy)
        {
            for (FSMState<Battle*>* state : states)
            {
                state->m_controller = this;
                m_stateMachine.AddState(state);
            }
        }

        ~Battle() { delete m_pArmy; }

        void Update(RtsGame& game, const WorldClock& clock) { m_stateMachine.Update(game, clock); }
        bool Active() const { return !m_stateMachine.ReachedGoalState(); }
        TID NextTarget() const { return m_nextTarget; }
        void NextTarget(TID nextTarget) { _ASSERTE(nextTarget >= 0); m_nextTarget = nextTarget; }
        TID CurrentTarget() const { return m_currentTarget; }
        void CurrentTarget(TID currentTarget) { _ASSERTE(currentTarget >= 0); m_currentTarget = currentTarget; }
        Army* GetArmy() const { return m_pArmy; }

    private:
        TID m_nextTarget;
        TID m_currentTarget;
        FSMMachine<Battle*> m_stateMachine;
        Army* m_pArmy;
    };

    template<>
    struct ControllerTraits<Battle*>
    {
        typedef Battle* Type;
        typedef const Battle* ConstType;
        static Army* GetArmy(ConstType battle) { return battle->GetArmy(); }
        static void NextTarget(Type battle, TID targetId) { battle->NextTarget(targetId); }
        static void CurrentTarget(Type battle, TID targetId) { battle->CurrentTarget(targetId); }
        static TID NextTarget(ConstType battle) { return battle->NextTarget(); }
        static TID CurrentTarget(ConstType battle) { return battle->CurrentTarget(); }
    };
}

#endif // BATTLE_H