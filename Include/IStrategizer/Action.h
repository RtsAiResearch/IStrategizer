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
        void State(ExecutionStateType p_state, RtsGame* pRtsGame, const WorldClock& p_clock);
        virtual bool ExecuteAux(RtsGame* pRtsGame, const WorldClock& p_clock) = 0;
        virtual void InitializePreConditions() = 0;
        virtual void OnSucccess(RtsGame* pRtsGame, const WorldClock& p_clock) {};
        virtual void OnFailure(RtsGame* pRtsGame, const WorldClock& p_clock) {};

    public:
        int Type() const { return PlanStepEx::_stepTypeId; }
        void UpdateAux(RtsGame* pRtsGame, const WorldClock& p_clock);
        void Reset(RtsGame* pRtsGame, const WorldClock& p_clock);
        void InitializeConditions();
        void Copy(IClonable* p_dest);
        virtual bool Execute(RtsGame* pRtsGame, const WorldClock& p_clock);
        virtual bool AliveConditionsSatisfied(RtsGame* pRtsGame) = 0;
        bool PreconditionsSatisfied(RtsGame* pRtsGame) { if (_preCondition == nullptr) InitializeConditions(); return _preCondition->Evaluate(pRtsGame); }
    };
}

#endif
