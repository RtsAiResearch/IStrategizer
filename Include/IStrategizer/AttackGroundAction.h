///> [Serializable]
#ifndef ATTACKGROUNDACTION_H
#define ATTACKGROUNDACTION_H

#include "Action.h"
#ifndef AND_H
#include "And.h"
#endif

#include "Vector2.h"

namespace IStrategizer
{
    class CellFeature;
    
    ///> class=AttackGroundAction
    ///> parent=Action
    class AttackGroundAction : public Action
    {
    public:
        OBJECT_SERIALIZABLE(AttackGroundAction);

        AttackGroundAction();
        AttackGroundAction(const PlanStepParameters& p_parameters);
        bool AliveConditionsSatisfied(RtsGame& p_RtsGame);
        bool SuccessConditionsSatisfied(RtsGame& p_RtsGame);

    protected:
        bool ExecuteAux(RtsGame& p_RtsGame, const WorldClock& p_clock);
        void InitializeAddressesAux() ;
        void HandleMessage(RtsGame& p_RtsGame, Message* p_msg, bool& p_consumed);
        void InitializePostConditions();
        void InitializePreConditions(RtsGame& p_RtsGame);

    private:
        TID _attackerId;
        Vector2 _position;
    };
}

#endif // ATTACKGROUNDACTION_H
