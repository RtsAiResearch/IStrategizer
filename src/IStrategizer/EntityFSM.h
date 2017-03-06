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
        EntityState(FSMStateTypeID typeId, const char* pName, EntityController* pController) :
            FSMState(typeId, pName, (EngineObject*)pController),
            m_targetPos1(Vector2::Inf()),
            m_targetEntity(INVALID_TID)
        {}

        Vector2 TargetPosition() const { return m_targetPos1; }
        TID TargetEntity() const { return m_targetEntity; }
        void Enter();
        void Exit();
        void DebugDraw();

    protected:
        Vector2 m_targetPos1;
        TID m_targetEntity;
    };

    class IdleEntityState : public EntityState
    {
    public:
        static const FSMStateTypeID TypeID = 0xE2D7944B;

        IdleEntityState(EntityController* pController) :
            EntityState(TypeID, "Idle", pController)
        {}

    private:
        DISALLOW_COPY_AND_ASSIGN(IdleEntityState);
    };

    class ArriveEntityState : public EntityState
    {
    public:
        static const FSMStateTypeID TypeID = 0x274E49CA;

        ArriveEntityState(EntityController* pController) :
            EntityState(TypeID, "Arrive", pController)
        {}

        void DebugDraw();
        void Update();

    private:
        DISALLOW_COPY_AND_ASSIGN(ArriveEntityState);
    };

    class FleeEntityState : public EntityState
    {
    public:
        static const FSMStateTypeID TypeID = 0x1B646F8D;

        FleeEntityState(EntityController* pController) :
            EntityState(TypeID, "Flee", pController)
        {}

        void Enter();
        void Update();
        void DebugDraw();

    private:
        DISALLOW_COPY_AND_ASSIGN(FleeEntityState);
    };

    class AttackEntityState : public EntityState
    {
    public:
        static const FSMStateTypeID TypeID = 0x6B18AF0C;

        AttackEntityState(EntityController* pController) :
            EntityState(TypeID, "Attack", pController)
        {}

        void Update();

    private:
        DISALLOW_COPY_AND_ASSIGN(AttackEntityState);
    };

    class AlarmEntityState : public EntityState
    {
    public:
        static const FSMStateTypeID TypeID = 0xE3068D3A;

        AlarmEntityState(EntityController* pController) :
            EntityState(TypeID, "Alarm", pController)
        {}

        void DebugDraw();

    private:
        DISALLOW_COPY_AND_ASSIGN(AlarmEntityState);
    };

    class RetreatEntityState : public EntityState
    {
    public:
        static const FSMStateTypeID TypeID = 0x6E8E8A14;

        RetreatEntityState(EntityController* pController) :
            EntityState(TypeID, "Retreat", pController)
        {}

        void DebugDraw();
        void Update();

    private:
        DISALLOW_COPY_AND_ASSIGN(RetreatEntityState);
    };

    class RepairEntityState : public EntityState
    {
    public:
        static const FSMStateTypeID TypeID = 0x76D4C5EC;

        RepairEntityState(EntityController* pController) :
            EntityState(TypeID, "Repair", pController)
        {}

        void Enter();
        void Update();

    private:
        DISALLOW_COPY_AND_ASSIGN(RepairEntityState);
    };

    class IdleEntityFSM : public StackFSM
    {
    public:
        static const FSMStateTypeID TypeID = 0x081ADAD2;

        IdleEntityFSM(EntityController* pController) :
            StackFSM("Entity-Idle", IdleEntityState::TypeID, IdleEntityState::TypeID, TypeID, (EngineObject*)pController)
        {
            AddState(FSMStatePtr(new IdleEntityState(pController)));
        }

        void CheckTransitions() {}

    private:
        DISALLOW_COPY_AND_ASSIGN(IdleEntityFSM);
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
            StackFSM("Entity-Scout", ArriveEntityState::TypeID, IdleEntityState::TypeID, TypeID, (EngineObject*)pController),
            m_scoutGoal(scoutGoal),
            m_currTargetPosIdx(0)
        {
            AddState(FSMStatePtr(new IdleEntityState(pController)));
            AddState(FSMStatePtr(new ArriveEntityState(pController)));
            AddState(FSMStatePtr(new FleeEntityState(pController)));
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

    class HintNRunEntityFSM : public StackFSM
    {
    public:
        static const FSMStateTypeID TypeID = 0xC5EEF702;

        HintNRunEntityFSM(EntityController* pController) :
            StackFSM("Entity-HnR", AlarmEntityState::TypeID, IdleEntityState::TypeID, TypeID, (EngineObject*)pController)
        {
            AddState(FSMStatePtr(new FleeEntityState(pController)));
            AddState(FSMStatePtr(new AttackEntityState(pController)));
            AddState(FSMStatePtr(new IdleEntityState(pController)));
            AddState(FSMStatePtr(new AlarmEntityState(pController)));
            AddState(FSMStatePtr(new RetreatEntityState(pController)));
        }

        void CheckTransitions();

    protected:
        HintNRunEntityFSM(const char* pName, FSMStateTypeID initialState, FSMStateTypeID finalState, FSMStateTypeID typeId, EngineObject* pController) :
            StackFSM(pName, initialState, finalState, typeId, pController)
        {}

    private:
        DISALLOW_COPY_AND_ASSIGN(HintNRunEntityFSM);
    };

    class GuardEntityFSM : public StackFSM
    {
    public:
        static const FSMStateTypeID TypeID = 0x1D672B0D;

        GuardEntityFSM(EntityController* pController) :
            StackFSM("Entity-Guard", AlarmEntityState::TypeID, IdleEntityState::TypeID, TypeID, (EngineObject*)pController)
        {
            AddState(FSMStatePtr(new IdleEntityState(pController)));
            AddState(FSMStatePtr(new ArriveEntityState(pController)));
            AddState(FSMStatePtr(new FleeEntityState(pController)));
            AddState(FSMStatePtr(new AttackEntityState(pController)));
            AddState(FSMStatePtr(new AlarmEntityState(pController)));
        }

        void CheckTransitions();

    private:
        DISALLOW_COPY_AND_ASSIGN(GuardEntityFSM);
    };

    class AutoRepairEntityFSM : public StackFSM
    {
    public:
        static const FSMStateTypeID TypeID = 0x76D4C5EC;

        AutoRepairEntityFSM(EntityController* pController) :
            StackFSM("Entity-AutoRepair", IdleEntityState::TypeID, IdleEntityState::TypeID, TypeID, (EngineObject*)pController)
        {
            AddState(FSMStatePtr(new IdleEntityState(pController)));
            AddState(FSMStatePtr(new RepairEntityState(pController)));
        }

        void CheckTransitions();

    private:
        DISALLOW_COPY_AND_ASSIGN(AutoRepairEntityFSM);
    };
}

#endif // !ENTITYFSM_H
