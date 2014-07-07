#ifndef BATTLE_H
#define BATTLE_H

#include "EngineComponent.h"
#include "EngineData.h"
#include "FSMMachine.h"
#include <vector>
#include <set>

namespace IStrategizer
{
    class RtsGame;
    class WorldClock;
    template<class T>
    struct ControllerTraits<T*>;

    class Battle : public EngineComponent
    {
    public:
        Battle(RtsGame& game);
        ~Battle();
        void NotifyMessegeSent(Message* p_msg);
        void Update(RtsGame& game, const WorldClock& clock) { m_stateMachine.Update(game, clock); }
        bool Active() const { return !m_stateMachine.ReachedGoalState(); }
        TID NextTarget() const { return m_nextTarget; }
        void NextTarget(TID nextTarget) { _ASSERTE(nextTarget >= 0); m_nextTarget = nextTarget; }
        TID CurrentTarget() const { return m_currentTarget; }
        void CurrentTarget(TID currentTarget) { _ASSERTE(currentTarget >= 0); m_currentTarget = currentTarget; }
        EntitySet Army() const { return m_army; }

    private:
        void SelectArmy(RtsGame& game);

        TID m_nextTarget;
        TID m_currentTarget;
        FSMMachine<Battle*> m_stateMachine;
        EntitySet m_army;
        EntityList m_destroyedArmyEntities;
        EntityList m_destroyedEnemyEntities;
    };

    template<>
    struct ControllerTraits<Battle*>
    {
        typedef Battle* Type;
        typedef const Battle* ConstType;
        static EntitySet Army(ConstType battle) { return battle->Army(); }
        static void NextTarget(Type battle, TID targetId) { battle->NextTarget(targetId); }
        static void CurrentTarget(Type battle, TID targetId) { battle->CurrentTarget(targetId); }
        static TID NextTarget(ConstType battle) { return battle->NextTarget(); }
        static TID CurrentTarget(ConstType battle) { return battle->CurrentTarget(); }
    };
}

#endif // BATTLE_H