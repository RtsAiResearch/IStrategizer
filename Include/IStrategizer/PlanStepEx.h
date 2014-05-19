///> [Serializable]
#ifndef PLANSTEPEX_H
#define PLANSTEPEX_H

#ifndef COMPOSITEEXPRESSION_H
#include "CompositeExpression.h"
#endif
#ifndef ENGINEDATA_H
#include "EngineData.h"
#endif
#ifndef ICOMPARABLE_H
#include "IComparable.h"
#endif
#ifndef USEROBJECT_H
#include "UserObject.h"
#endif
#ifndef ICLONABLE_H
#include "IClonable.h"
#endif
#ifndef CELLFEATURE_H
#include "CellFeature.h"
#endif
#ifndef WORLDCLOCK_H
#include "WorldClock.h"
#endif
#ifndef RTSGAME_H
#include "RtsGame.h"
#endif

#include <map>
#include <vector>
#include <string>

namespace IStrategizer
{
    class ConditionEx;
    class Message;

    ///> class=PlanStepEx
    class PlanStepEx : public Serialization::UserObject, public IComparable, public IClonable
    {
        OBJECT_MEMBERS(2, &_params, &_id);

    public:
        void Parameters(const PlanStepParameters& p_val) { _params.insert(p_val.begin(), p_val.end()) ; }
        void Copy(IClonable* p_dest);
        void Update(RtsGame& game, const WorldClock& p_clock);
        int StepTypeId() const { return _stepTypeId; }
        int Parameter(int p_parameterName) { return _params[(ParameterType)p_parameterName]; }
        int Compare(IComparable* p_rhs) { return !Equals((PlanStepEx*)p_rhs); }
        bool Equals(PlanStepEx* p_planStep);
        const PlanStepParameters& Parameters() const { return _params; }
        virtual void HandleMessage(RtsGame& game, Message* p_msg, bool& p_consumed) {}
        virtual void InitializeConditions();
        virtual bool SuccessConditionsSatisfied(RtsGame& game) = 0;
        virtual void UpdateAux(RtsGame& game, const WorldClock& p_clock) = 0;
        virtual void Reset(RtsGame& game, const WorldClock& p_clock) = 0;
        virtual std::string ToString(bool minimal = false) const;
        virtual ~PlanStepEx() {}
        PlanStepParameters& Parameters() { return _params; }
        ExecutionStateType State() const { return _state; }
        virtual void State(ExecutionStateType p_state, RtsGame& game, const WorldClock& p_clock);
        StepLevelType LevelType() const { return _stepLevelType; }
        CompositeExpression* PostCondition() { _ASSERTE(_postCondition); return _postCondition; }
        IClonable* Clone();
        unsigned Id() const { return _id; }
        void Id(unsigned id) { _id = id; }

    protected:
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
}

#endif // PLANSTEPEX_H
