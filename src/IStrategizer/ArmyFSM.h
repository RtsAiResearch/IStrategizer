#ifndef ARMYFSM_H
#define ARMYFSM_H

#include "StackFSM.h"
#include "EntityController.h"
#include "ArmyController.h"

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

        Vector2 TargetPosition() const { return m_targetPos1; }
        Vector2 TargetPosition2() const { return m_targetPos2; }
        TID TargetEntity() const { return m_targetEntity; }
        const EntityControllersMap& Entities() const { return m_controlledEntities; }
        void Update();
        void Enter();
        void Exit();
        void DebugDraw();

    protected:
        Vector2 m_targetPos1;
        Vector2 m_targetPos2;
        TID m_targetEntity;
        EntityControllersMap m_controlledEntities;
    };

    class IdleArmyState : public ArmyState
    {
    public:
        static const FSMStateTypeID TypeID = 0xC8ED84B8;

        IdleArmyState(ArmyController* pController) :
            ArmyState(TypeID, "[IDLE]", pController)
        {}

    private:
        DISALLOW_COPY_AND_ASSIGN(IdleArmyState);
    };

    class AlarmArmyState : public ArmyState
    {
    public:
        static const FSMStateTypeID TypeID = 0xCEB67D75;

        AlarmArmyState(ArmyController* pController) :
            ArmyState(TypeID, "[ALARM]", pController)
        {}

    private:
        DISALLOW_COPY_AND_ASSIGN(AlarmArmyState);
    };

    class AttackArmyState : public ArmyState
    {
    public:
        static const FSMStateTypeID TypeID = 0x1825A7EA;

        AttackArmyState(ArmyController* pController) :
            ArmyState(TypeID, "[ATTACK]", pController)
        {}

    private:
        DISALLOW_COPY_AND_ASSIGN(AttackArmyState);
    };

    class RegroupArmyState : public ArmyState
    {
    public:
        static const FSMStateTypeID TypeID = 0x4EFF3C4E;

        RegroupArmyState(bool regrouAtArmyCenter, ArmyController* pController) :
            ArmyState(TypeID, "[REGROUP]", pController),
            m_regroupArea(Circle2(Vector2::Inf(), INT_MAX)),
            m_regroupAtArmyCenter(regrouAtArmyCenter)
        {}

        void Enter();
        void Update();
        void DebugDraw();

    private:
        DISALLOW_COPY_AND_ASSIGN(RegroupArmyState);

        ArmyGroupFormation m_formation;
        Circle2 m_regroupArea;
        bool m_regroupAtArmyCenter;
    };

    class ArriveArmyState : public ArmyState
    {
    public:
        static const FSMStateTypeID TypeID = 0x2DABAA04;

        ArriveArmyState(ArmyController* pController) :
            ArmyState(TypeID, "[ARRIVE]", pController),
            m_arriveArea()
        {}

        void Enter();
        void Update();
        void DebugDraw();

    private:
        DISALLOW_COPY_AND_ASSIGN(ArriveArmyState);

        Circle2 m_arriveArea;
    };

    class IdleArmyFSM : public StackFSM
    {
    public:
        static const FSMStateTypeID TypeID = 0x5DFE949F;
        IdleArmyFSM(ArmyController* pController) :
            StackFSM("ARMY-IDLE",IdleArmyState::TypeID, IdleArmyState::TypeID, TypeID, (EngineObject*)pController)
        {
            AddState(FSMStatePtr(new IdleArmyState(pController)));
        }

        void CheckTransitions() {}

    private:
        DISALLOW_COPY_AND_ASSIGN(IdleArmyFSM);
    };

    class StandArmyFSM : public StackFSM
    {
    public:
        static const FSMStateTypeID TypeID = 0x488AD1FD;
        StandArmyFSM(ArmyController* pController) :
            StackFSM("ARMY-STAND", IdleArmyState::TypeID, IdleArmyState::TypeID, TypeID, (EngineObject*)pController)
        {
            AddState(FSMStatePtr(new IdleArmyState(pController)));
            AddState(FSMStatePtr(new RegroupArmyState(false, pController)));
        }

        void CheckTransitions();

    private:
        DISALLOW_COPY_AND_ASSIGN(StandArmyFSM);
    };

    class GuardArmyFSM : public StackFSM
    {
    public:
        static const FSMStateTypeID TypeID = 0x719CB920;
        GuardArmyFSM(ArmyController* pController) :
            StackFSM("ARMY-GUARD", AlarmArmyState::TypeID, AlarmArmyState::TypeID, TypeID, (EngineObject*)pController)
        {
            AddState(FSMStatePtr(new AlarmArmyState(pController)));
            AddState(FSMStatePtr(new AttackArmyState(pController)));
            AddState(FSMStatePtr(new RegroupArmyState(false, pController)));
        }

        void CheckTransitions();

    private:
        DISALLOW_COPY_AND_ASSIGN(GuardArmyFSM);
    };

    class AttackMoveArmyFSM : public StackFSM
    {
    public:
        static const FSMStateTypeID TypeID = 0x47546124;
        AttackMoveArmyFSM(ArmyController* pController) :
            StackFSM("ARMY-ATTCK-MOVE", ArriveArmyState::TypeID, AlarmArmyState::TypeID, TypeID, (EngineObject*)pController)
        {
            AddState(FSMStatePtr(new AlarmArmyState(pController)));
            AddState(FSMStatePtr(new AttackArmyState(pController)));
            AddState(FSMStatePtr(new ArriveArmyState(pController)));
            AddState(FSMStatePtr(new RegroupArmyState(true, pController)));
        }

        void CheckTransitions();

    private:
        DISALLOW_COPY_AND_ASSIGN(AttackMoveArmyFSM);
    };
}

#endif // ARMYFSM_H