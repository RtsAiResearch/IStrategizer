///> [Serializable]
#ifndef PLANSTEPEX_H
#define PLANSTEPEX_H

#include <map>
#include <vector>
#include <string>
#include <memory>
#include "CompositeExpression.h"
#include "EngineData.h"
#include "IComparable.h"
#include "EngineObject.h"
#include "IClonable.h"
#include "CellFeature.h"
#include "WorldClock.h"
#include "RtsGame.h"

namespace IStrategizer
{
    class ConditionEx;
    class Message;

    ///> class=PlanStepEx
	class PlanStepEx : public EngineObject, public IComparable, public IClonable
    {
        OBJECT_SERIALIZABLE(PlanStepEx, &_params, &_id);
    public:
        static unsigned GenerateID();
        
        void Parameters(const PlanStepParameters& p_val) { _params.insert(p_val.begin(), p_val.end()) ; }
        void Copy(IClonable* p_dest);
        virtual void Update(RtsGame& game, const WorldClock& p_clock);
        int StepTypeId() const { return _stepTypeId; }
        int Parameter(int p_parameterName) const { return ContainsParameter(p_parameterName) ? _params.at((ParameterType)p_parameterName) : 0; }
        int ContainsParameter(int p_parameterName) const { return _params.find((ParameterType)p_parameterName) != _params.end(); }
        int Compare(IComparable* p_rhs) { return !Equals((PlanStepEx*)p_rhs); }
        const PlanStepParameters& Parameters() const { return _params; }
        virtual void HandleMessage(RtsGame& game, Message* p_msg, bool& p_consumed) {}
        virtual void InitializeConditions();
        virtual void UpdateAux(RtsGame& game, const WorldClock& p_clock) {}
        virtual void Reset(RtsGame& game, const WorldClock& p_clock) = 0;
        virtual void State(ExecutionStateType p_state, RtsGame& game, const WorldClock& p_clock);
        virtual bool Equals(PlanStepEx* p_planStep) = 0;
        virtual bool SuccessConditionsSatisfied(RtsGame& game) = 0;
        virtual unsigned Hash(bool quantified = true) const;
        virtual std::string ToString(bool minimal = false) const;
        virtual ~PlanStepEx() {}
        PlanStepParameters& Parameters() { return _params; }
        ExecutionStateType State() const { return _state; }
        StepLevelType LevelType() const { return _stepLevelType; }
        CompositeExpression* PostCondition() { _ASSERTE(_postCondition); return _postCondition; }
        IClonable* Clone();
        unsigned Id() const { return _id; }
        void Id(unsigned id) { _id = id; }

    protected:
        PlanStepEx();
        ///> type=PlanStepParameters
        PlanStepParameters _params;
        ///> type=int
        unsigned _id;
        int _stepTypeId;
        StepLevelType _stepLevelType;
        CompositeExpression* _postCondition;
        unsigned _stateStartTime[COUNT(ExecutionStateType)];
        unsigned _stateTimeout[COUNT(ExecutionStateType)];
        bool _firstUpdate;
        static unsigned s_lastPlanstepID;

        PlanStepEx(int p_stepTypeId, ExecutionStateType p_state);
        PlanStepEx(int p_stepTypeId, ExecutionStateType p_state, const PlanStepParameters& p_parameters);
        bool IsCurrentStateTimeout(const WorldClock& p_clock);
        virtual void InitializePostConditions() = 0;

    private:
        ExecutionStateType _state;
    };
    
    typedef std::shared_ptr<PlanStepEx> PlanStepStrongPtr;
    typedef std::shared_ptr<const PlanStepEx> ConstPlanStepStrongPtr;
    typedef std::weak_ptr<PlanStepEx> PlanStepWeakPtr;
}

#endif // PLANSTEPEX_H
