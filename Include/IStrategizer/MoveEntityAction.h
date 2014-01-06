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
        bool AliveConditionsSatisfied(RtsGame* pRtsGame);
        bool SuccessConditionsSatisfied(RtsGame* pRtsGame);

    protected:
        void OnSucccess(RtsGame* pRtsGame, const WorldClock& p_clock);
        void OnFailure(RtsGame* pRtsGame, const WorldClock& p_clock);
        void HandleMessage(RtsGame *pRtsGame, Message* p_msg, bool& p_consumed);
        void InitializePostConditions();
        void InitializePreConditions();
        bool ExecuteAux(RtsGame* pRtsGame, const WorldClock& p_clock);

    private:
        TID _entityId;
        Vector2 _position;
    };
}

#endif // MOVEENTITYACTION_H
