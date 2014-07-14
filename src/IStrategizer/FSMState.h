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
        static Army* GetArmy(ConstType& v) { throw "Not implemented exception"; }
        static void NextTarget(Type v, TID targetEnemyEntity) { throw "Not implemented exception" }
        static TID NextTarget(ConstType battle) { throw "Not implemented exception" }
        static void CurrentTarget(Type v, TID targetEnemyEntity) { throw "Not implemented exception" }
        static TID CurrentTarget(ConstType battle) { throw "Not implemented exception" }
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

        TController m_controller;

    private:
        int m_type;
    };
}

#endif // FSMSTATE_H