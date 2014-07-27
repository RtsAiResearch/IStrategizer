#ifndef ARMYFSM_H
#define ARMYFSM_H

#include "StackFSM.h"

namespace IStrategizer
{
    class ArmyController;


    class ArmyState : public FSMState
    {
    public:
        ArmyState(FSMStateTypeID typeId, const char* pName, ArmyController* pController) :
            FSMState(typeId, pName, (EngineObject*)pController),
            m_targetPos1(Vector2::Inf()),
            m_targetPos2(Vector2::Inf()),
            m_targetEntity(INVALID_TID)
        {}

        Vector2 TargetPosition1() const { return m_targetPos1; }
        Vector2 TargetPosition2() const { return m_targetPos2; }
        TID TargetEntity() const { return m_targetEntity; }
        void Update();
        void Enter();

    protected:
        Vector2 m_targetPos1;
        Vector2 m_targetPos2;
        TID m_targetEntity;
    };

    class IdleArmyState : public ArmyState
    {
    public:
        static const FSMStateTypeID TypeID = 0xC8ED84B8;

        IdleArmyState(ArmyController* pController) :
            ArmyState(TypeID, "IDLE", pController)
        {}

    private:
        DISALLOW_COPY_AND_ASSIGN(IdleArmyState);
    };

    class AlarmArmyState : public ArmyState
    {
    public:
        static const FSMStateTypeID TypeID = 0xCEB67D75;

        AlarmArmyState(ArmyController* pController) :
            ArmyState(TypeID, "ALARM", pController)
        {}

    private:
        DISALLOW_COPY_AND_ASSIGN(AlarmArmyState);
    };

    class AttackArmyState : public ArmyState
    {
    public:
        static const FSMStateTypeID TypeID = 0x1825A7EA;

        AttackArmyState(ArmyController* pController) :
            ArmyState(TypeID, "ATTACK", pController)
        {}

        void Enter();
        void Exist();

    private:
        DISALLOW_COPY_AND_ASSIGN(AttackArmyState);
    };

    class RegroupArmyState : public ArmyState
    {
    public:
        static const FSMStateTypeID TypeID = 0x4EFF3C4E;

        RegroupArmyState(ArmyController* pController) :
            ArmyState(TypeID, "REGROUP", pController)
        {}

        void Enter();
        void Update();

    private:
        DISALLOW_COPY_AND_ASSIGN(RegroupArmyState);
    };

    class IdleArmyFSM : public StackFSM
    {
    public:
        static const FSMStateTypeID TypeID = 0x5DFE949F;
        IdleArmyFSM(ArmyController* pController) :
            StackFSM(IdleArmyState::TypeID, IdleArmyState::TypeID, TypeID, (EngineObject*)pController)
        {
            AddState(FSMStatePtr(new IdleArmyState(pController)));
        }

        void CheckTransitions() {}

    private:
        DISALLOW_COPY_AND_ASSIGN(IdleArmyFSM);
    };

    class GuardArmyFSM : public StackFSM
    {
    public:
        static const FSMStateTypeID TypeID = 0x719CB920;
        GuardArmyFSM(ArmyController* pController) :
            StackFSM(AlarmArmyState::TypeID, AlarmArmyState::TypeID, TypeID, (EngineObject*)pController)
        {
            AddState(FSMStatePtr(new AlarmArmyState(pController)));
            AddState(FSMStatePtr(new AttackArmyState(pController)));
            AddState(FSMStatePtr(new RegroupArmyState(pController)));
        }

        void CheckTransitions();

    private:
        DISALLOW_COPY_AND_ASSIGN(GuardArmyFSM);
    };
}

#endif // ARMYFSM_H