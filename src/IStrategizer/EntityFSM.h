#ifndef ENTITYFSM_H
#define ENTITYFSM_H

#include "EngineDefs.h"
#include "FSMState.h"
#include "StackFSM.h"
#include "EntityController.h"

namespace IStrategizer
{
    class EntityController;

    class EntityState : public FSMState
    {
    public:
        EntityState(FSMStateTypeID typeId, EntityController* pController) :
            FSMState(typeId, (EngineObject*)pController),
            m_targetPos1(Vector2::Inf()),
            m_targetPos2(Vector2::Inf()),
            m_targetEntity(INVALID_TID)
        {}

        Vector2 TargetPosition1() const { return m_targetPos1; }
        Vector2 TargetPosition2() const { return m_targetPos2; }
        TID TargetEntity() const { return m_targetEntity; }
        void Update();

    protected:
        Vector2 m_targetPos1;
        Vector2 m_targetPos2;
        TID m_targetEntity;
    };

    class IdleState : public EntityState
    {
    public:
        static const FSMStateTypeID TypeID = 0xE2D7944B;

        IdleState(EntityController* pController) :
            EntityState(TypeID, pController)
        {}

    private:
        DISALLOW_COPY_AND_ASSIGN(IdleState);
    };

    class ArriveState : public EntityState
    {
    public:
        static const FSMStateTypeID TypeID = 0x274E49CA;

        ArriveState(EntityController* pController) :
            EntityState(TypeID, pController)
        {}

        void Enter();
        void Update();

    private:
        DISALLOW_COPY_AND_ASSIGN(ArriveState);
    };

    class FleeState : public EntityState
    {
    public:
        static const FSMStateTypeID TypeID = 0x1B646F8D;

        FleeState(EntityController* pController) :
            EntityState(TypeID, pController)
        {}

        void Enter();
        void Update();

    private:
        DISALLOW_COPY_AND_ASSIGN(FleeState);
    };

    class AttackState : public EntityState
    {
    public:
        static const FSMStateTypeID TypeID = 0x6B18AF0C;

        AttackState(EntityController* pController) :
            EntityState(TypeID, pController)
        {}

        void Enter();
        void Update();

    private:
        DISALLOW_COPY_AND_ASSIGN(AttackState);
    };

    class AlarmState : public EntityState
    {
    public:
        static const FSMStateTypeID TypeID = 0xE3068D3A;

        AlarmState(EntityController* pController) :
            EntityState(TypeID, pController)
        {}

        void Enter();
        void Update();

    private:
        DISALLOW_COPY_AND_ASSIGN(AlarmState);
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

    class GuardEntityFSM : public StackFSM
    {
    public:
        static const FSMStateTypeID TypeID = 0x1D672B0D;

        GuardEntityFSM(EntityController* pController) :
            StackFSM(AlarmState::TypeID, IdleState::TypeID, TypeID, (EngineObject*)pController)
        {
            AddState(FSMStatePtr(new IdleState(pController)));
            AddState(FSMStatePtr(new ArriveState(pController)));
            AddState(FSMStatePtr(new FleeState(pController)));
            AddState(FSMStatePtr(new AttackState(pController)));
            AddState(FSMStatePtr(new AlarmState(pController)));
        }

        void CheckTransitions();

    private:
        DISALLOW_COPY_AND_ASSIGN(GuardEntityFSM);
    };
}

#endif // !ENTITYFSM_H
