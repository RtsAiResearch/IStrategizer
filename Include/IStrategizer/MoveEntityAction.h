///> [Serializable]
#ifndef MOVEENTITYACTION_H
#define MOVEENTITYACTION_H

#include "Action.h"
#ifndef VECTOR2_H
#include "Vector2.h"
#endif

namespace IStrategizer
{
    ///> class=MoveEntityAction
    ///> parent=Action
    class MoveEntityAction : public Action
    {
        OBJECT_SERIALIZABLE(MoveEntityAction);

    public:
        MoveEntityAction();
        MoveEntityAction(const PlanStepParameters& p_parameters);
        bool AliveConditionsSatisfied(RtsGame& p_RtsGame);
        bool SuccessConditionsSatisfied(RtsGame& p_RtsGame);

    protected:
        void OnSucccess(RtsGame& p_RtsGame, const WorldClock& p_clock);
        void OnFailure(RtsGame& p_RtsGame, const WorldClock& p_clock);
        void HandleMessage(RtsGame& p_RtsGame, Message* p_msg, bool& p_consumed);
        void InitializePostConditions();
        void InitializePreConditions(RtsGame& p_RtsGame);
        bool ExecuteAux(RtsGame& p_RtsGame, const WorldClock& p_clock);

    private:
        TID _entityId;
        Vector2 _position;
    };
}

#endif // MOVEENTITYACTION_H
