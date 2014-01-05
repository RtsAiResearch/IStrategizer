///> [Serializable]
#ifndef ATTACKENTITYACTION_H
#define ATTACKENTITYACTION_H

#include "Action.h"
#include "CellFeature.h"

#ifndef CHECKPOSITIONFILTERCOUNT_H
#include "CheckPositionFilterCount.h"
#endif
#ifndef AND_H
#include "And.h"
#endif

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
        bool PreconditionsSatisfied();
        bool AliveConditionsSatisfied();
        bool SuccessConditionsSatisfied();

    protected:
        bool ExecuteAux(const WorldClock& p_clock );
        void InitializeAddressesAux() ;
        void HandleMessage(Message* p_pMsg, bool& p_consumed);
        void InitializePostConditions();
        void InitializePreConditions();

    private:
        TID _attackerId;
        TID _targetId;
        Vector2 _position;
    };
}

#endif // ATTACKENTITYACTION_H
