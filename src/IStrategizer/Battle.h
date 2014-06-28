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
        void NotifyMessegeSent(Message* p_msg);
        void Update(RtsGame& game, const WorldClock& clock) { m_stateMachine.Update(game, clock); }
        bool Active() const { return !m_stateMachine.ReachedGoalState(); }
        TID TargetEnemyEntity() const { return m_targetEnemyEntity; }
        void TargetEnemyEntity(TID targetEnemyEntity) { _ASSERTE(targetEnemyEntity >= 0); m_targetEnemyEntity = targetEnemyEntity; }
        EntitySet Army() const { return m_army; }

    private:
        void SelectArmy(RtsGame& game);

        TID m_targetEnemyEntity;
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
        static void TargetEnemyEntity(Type battle, TID targetEnemyEntity) { battle->TargetEnemyEntity(targetEnemyEntity); }
        static TID TargetEnemyEntity(ConstType battle) { return battle->TargetEnemyEntity(); }
    };
}

#endif // BATTLE_H