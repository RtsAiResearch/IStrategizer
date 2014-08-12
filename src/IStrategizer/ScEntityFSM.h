#ifndef STARCRAFTENTITYFSM_H
#define STARCRAFTENTITYFSM_H

#include "StackFSM.h"
#include "EntityFSM.h"

namespace IStrategizer
{
    class EntityController;

    class PlantSpiderMinesState : public EntityState
    {
    public:
        static const FSMStateTypeID TypeID = 0x7AD7151C;

        PlantSpiderMinesState(EntityController* pController) :
            EntityState(TypeID, "PlantSpiderMines", pController)
        {}

        static bool IsMineOnTile(_In_ TID entityId);
        static int MinesCountInRegion(_In_ Vector2 pos);

        void Enter();

    private:
        DISALLOW_COPY_AND_ASSIGN(PlantSpiderMinesState);
    };

    class VultureHintNRunEntityFSM : public HintNRunEntityFSM
    {
    public:
        static const FSMStateTypeID TypeID = 0xA1F25857;

        VultureHintNRunEntityFSM(EntityController* pController) :
            HintNRunEntityFSM("Vulture-HnR", AlarmEntityState::TypeID, IdleEntityState::TypeID, TypeID, (EngineObject*)pController)
        {
            AddState(FSMStatePtr(new FleeEntityState(pController)));
            AddState(FSMStatePtr(new AttackEntityState(pController)));
            AddState(FSMStatePtr(new IdleEntityState(pController)));
            AddState(FSMStatePtr(new AlarmEntityState(pController)));
            AddState(FSMStatePtr(new RetreatEntityState(pController)));
            AddState(FSMStatePtr(new PlantSpiderMinesState(pController)));
        }

        void CheckTransitions();

    private:
        DISALLOW_COPY_AND_ASSIGN(VultureHintNRunEntityFSM);
    };
}

#endif // STARCRAFTENTITYFSM_H