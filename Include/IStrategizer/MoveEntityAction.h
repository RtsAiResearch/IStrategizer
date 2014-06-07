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
        bool AliveConditionsSatisfied(RtsGame& game);
        bool SuccessConditionsSatisfied(RtsGame& game);
        bool Equals(PlanStepEx* p_planStep) { return true; }
        unsigned Hash() { return 0; }

    protected:
        void OnSucccess(RtsGame& game, const WorldClock& p_clock);
        void OnFailure(RtsGame& game, const WorldClock& p_clock);
        void HandleMessage(RtsGame& game, Message* p_msg, bool& p_consumed);
        void InitializePostConditions();
        void InitializePreConditions();
        bool ExecuteAux(RtsGame& game, const WorldClock& p_clock);

    private:
        TID _entityId;
        Vector2 _position;
    };
}

#endif // MOVEENTITYACTION_H
