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
        bool AliveConditionsSatisfied(RtsGame* pRtsGame);
        bool SuccessConditionsSatisfied(RtsGame* pRtsGame);

    protected:
        bool ExecuteAux(RtsGame* pRtsGame, const WorldClock& p_clock);
        void InitializeAddressesAux();
        void HandleMessage(RtsGame *pRtsGame, Message* p_msg, bool& p_consumed);
        void InitializePostConditions();
        void InitializePreConditions();

    private:
        TID _attackerId;
        TID _targetId;
        Vector2 _position;
    };
}

#endif // ATTACKENTITYACTION_H
