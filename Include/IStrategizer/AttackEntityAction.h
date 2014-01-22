///> [Serializable]
#ifndef ATTACKENTITYACTION_H
#define ATTACKENTITYACTION_H

#include "Action.h"
#include "CellFeature.h"

namespace IStrategizer
{
    ///> class=AttackEntityAction
    ///> parent=Action
    class AttackEntityAction : public Action
    {
        OBJECT_SERIALIZABLE(AttackEntityAction);

    public:
        AttackEntityAction();
        AttackEntityAction(const PlanStepParameters& p_parameters);
        void Copy(IClonable* p_dest);
        bool AliveConditionsSatisfied(RtsGame& p_RtsGame);
        bool SuccessConditionsSatisfied(RtsGame& p_RtsGame);

    protected:
        bool ExecuteAux(RtsGame& p_RtsGame, const WorldClock& p_clock);
        void InitializeAddressesAux();
        void HandleMessage(RtsGame& p_RtsGame, Message* p_msg, bool& p_consumed);
        void InitializePostConditions();
        void InitializePreConditions(RtsGame& p_RtsGame);

    private:
        TID _attackerId;
        TID _targetId;
        Vector2 _position;
    };
}

#endif // ATTACKENTITYACTION_H
