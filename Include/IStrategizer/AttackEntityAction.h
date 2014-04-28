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
        bool AliveConditionsSatisfied(RtsGame& game);
        bool SuccessConditionsSatisfied(RtsGame& game);

    protected:
        bool ExecuteAux(RtsGame& game, const WorldClock& p_clock);
        void InitializeAddressesAux();
        void InitializePostConditions();
        void InitializePreConditions();
        void OnSucccess(RtsGame& game, const WorldClock& p_clock);
        void OnFailure(RtsGame& game, const WorldClock& p_clock);

    private:
        TID m_attackerId;
        TID m_targetId;
    };
}

#endif // ATTACKENTITYACTION_H
