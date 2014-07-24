#ifndef ENTITYFSM_H
#define ENTITYFSM_H

#include "EngineDefs.h"
#include "FSMState.h"
#include "StackFSM.h"

namespace IStrategizer
{
    class EntityController;

    class IdleState : public FSMState
    {
    public:
        static const FSMStateTypeID TypeID = 0xE2D7944B;

        IdleState(EntityController* pController) :
            FSMState(TypeID, (EngineObject*)pController)
        {}

    private:
        DISALLOW_COPY_AND_ASSIGN(IdleState);
    };

    class ArriveState : public FSMState
    {
    public:
        static const FSMStateTypeID TypeID = 0x274E49CA;

        ArriveState(EntityController* pController) :
            FSMState(TypeID, (EngineObject*)pController)
        {}

        void Enter();

    private:
        DISALLOW_COPY_AND_ASSIGN(ArriveState);
    };

    class FleeState : public FSMState
    {
    public:
        static const FSMStateTypeID TypeID = 0x1B646F8D;

        FleeState(EntityController* pController) :
            FSMState(TypeID, (EngineObject*)pController)
        {}

        void Enter();
        
    private:
        DISALLOW_COPY_AND_ASSIGN(FleeState);
    };

    class ScoutEntityFSM : public StackFSM
    {
    public:
        static const FSMStateTypeID TypeID = 0x8C39CAD8;

        ScoutEntityFSM(EntityController* pController) :
            StackFSM(ArriveState::TypeID, IdleState::TypeID, TypeID, (EngineObject*)pController)
        {
            AddState(FSMStatePtr(new IdleState(pController)));
            AddState(FSMStatePtr(new ArriveState(pController)));
            AddState(FSMStatePtr(new FleeState(pController)));
        }

        void CheckTransitions();

    private:
        DISALLOW_COPY_AND_ASSIGN(ScoutEntityFSM);
    };
}

#endif // !ENTITYFSM_H
