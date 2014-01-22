///> [Serializable]
#ifndef ACTION_H
#define ACTION_H

#include <map>
#include <vector>
#include "PlanStepEx.h"
#include "ErrorCode.h"
#include "MetaData.h"

namespace IStrategizer
{
    class CompositeExpression;

    ///> class=Action
    ///> parent=PlanStepEx
    class Action : public PlanStepEx
    {
    protected:
        CompositeExpression* _preCondition;

        Action(ActionType p_actionType, unsigned p_maxPrepTime = 0, unsigned p_maxExecTrialTime = 0, unsigned p_maxExecTime = 0);
        Action(ActionType p_actionType, const PlanStepParameters& p_parameters, unsigned p_maxPrepTime = 0,  unsigned p_maxExecTrialTime = 0, unsigned p_maxExecTime = 0);
        void State(ExecutionStateType p_state, RtsGame& p_RtsGame, const WorldClock& p_clock);
        void UpdateAux(RtsGame& p_RtsGame, const WorldClock& p_clock);
        virtual bool ExecuteAux(RtsGame& p_RtsGame, const WorldClock& p_clock) = 0;
        virtual void InitializePreConditions(RtsGame& p_RtsGame) = 0;
        virtual void OnSucccess(RtsGame& p_RtsGame, const WorldClock& p_clock) {};
        virtual void OnFailure(RtsGame& p_RtsGame, const WorldClock& p_clock) {};

    public:
        int Type() const { return PlanStepEx::_stepTypeId; }
        void Reset(RtsGame& p_RtsGame, const WorldClock& p_clock);
        void InitializeConditions(RtsGame& p_RtsGame);
        void Copy(IClonable* p_dest);
        virtual bool Execute(RtsGame& p_RtsGame, const WorldClock& p_clock);
        virtual bool AliveConditionsSatisfied(RtsGame& p_RtsGame) = 0;
        bool PreconditionsSatisfied(RtsGame& p_RtsGame);
    };
}

#endif
