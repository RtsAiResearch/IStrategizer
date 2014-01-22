///> [Serializable]
#ifndef MOVEACTION_H
#define MOVEACTION_H
#include "Action.h"
#include "Vector2.h"

 namespace IStrategizer 
{
    class GameEntity;

    ///> class=MoveAction
    ///> parent=Action
    class MoveAction:public Action
    {
    public:
        OBJECT_SERIALIZABLE(MoveAction);

        MoveAction();
        MoveAction(const PlanStepParameters& p_parameters);
        void Copy(IClonable* p_dest);
        bool AliveConditionsSatisfied(RtsGame& p_RtsGame);
        bool SuccessConditionsSatisfied(RtsGame& p_RtsGame);

    protected:
        bool ExecuteAux(RtsGame& p_RtsGame, const WorldClock& p_clock);
        void InitializeAddressesAux() ;
        void HandleMessage(RtsGame& p_RtsGame, Message* p_msg, bool& p_consumed);
        void InitializePostConditions();
        void InitializePreConditions(RtsGame& p_RtsGame);

    private:
        TID _entityId;
        Vector2 _position;
        GameEntity* _pEntity;
    };
}

#endif// ATTACKGROUNDACTION_H 
