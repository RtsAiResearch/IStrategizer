#ifndef FSMSTATE_H
#define FSMSTATE_H

#include "EngineData.h"
#include "EngineObject.h"
#include <set>

namespace IStrategizer
{
    class WorldClock;
    class RtsGame;

    template<class T>
    struct ControllerTraits
    {
        typedef T Type;
        typedef const T ConstType;
        static EntitySet Army(ConstType& v) { return set<TID>(); }
        static void NextTarget(Type v, TID targetEnemyEntity) { }
        static TID NextTarget(ConstType battle) { return default<TID>(); }
        static void CurrentTarget(Type v, TID targetEnemyEntity) { }
        static TID CurrentTarget(ConstType battle) { return default<TID>(); }
    };

    template<class TController, class TControllerTraits = ControllerTraits<TController>>
    class FSMState : public EngineObject
    {
    public:
        FSMState(int type, TController controller) : m_type(type), m_controller(controller) {}
        int StateType() { return m_type; }
        virtual void Enter(RtsGame& game, const WorldClock& clock) {}
        virtual void Exit(RtsGame& game, const WorldClock& clock) {}
        virtual void Update(RtsGame& game, const WorldClock& clock) {}
        virtual int CheckTransitions(RtsGame& game, const WorldClock& clock)  { return m_type; }

    protected:
        TController m_controller;

    private:
        int m_type;
    };
}

#endif // FSMSTATE_H