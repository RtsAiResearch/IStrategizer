#ifndef ENTITYFSM_H
#define ENTITYFSM_H

#include "EngineDefs.h"
#include "FSMState.h"
#include "StackFSM.h"
#include "EntityController.h"

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
        void Update();

    private:
        DISALLOW_COPY_AND_ASSIGN(ArriveState);
        Vector2 m_targetPos;
    };

    class FleeState : public FSMState
    {
    public:
        static const FSMStateTypeID TypeID = 0x1B646F8D;

        FleeState(EntityController* pController) :
            FSMState(TypeID, (EngineObject*)pController)
        {}

        void Enter();
        void Update();

    private:
        DISALLOW_COPY_AND_ASSIGN(FleeState);
        Vector2 m_targetPos;
    };

    class ScoutEntityFSM : public StackFSM
    {
    public:
        enum Goal
        {
            SCTGL_Explore,
            SCTGL_Visit
        };

        static const FSMStateTypeID TypeID = 0x8C39CAD8;

        ScoutEntityFSM(Goal scoutGoal, EntityController* pController) :
            StackFSM(ArriveState::TypeID, IdleState::TypeID, TypeID, (EngineObject*)pController),
            m_scoutGoal(scoutGoal),
            m_currTargetPosIdx(0)
        {
            AddState(FSMStatePtr(new IdleState(pController)));
            AddState(FSMStatePtr(new ArriveState(pController)));
            AddState(FSMStatePtr(new FleeState(pController)));
        }

        void Reset();
        void CheckTransitions();
        bool SetNextTargetPosition();
        bool MoreTargetsExist();

    private:
        DISALLOW_COPY_AND_ASSIGN(ScoutEntityFSM);
        Goal m_scoutGoal;
        int m_currTargetPosIdx;
    };
}

#endif // !ENTITYFSM_H
