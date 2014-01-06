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
    private:
        ExecutionStateType _state;

    protected:
        ///> type=PlanStepParameters
        PlanStepParameters _params;
        int _stepTypeId;
        unsigned _data;
        StepLevelType _stepLevelType;
        CompositeExpression* _postCondition;
        unsigned _stateStartTime[COUNT(ExecutionStateType)];
        unsigned _stateTimeout[COUNT(ExecutionStateType)];
        bool _firstUpdate;

        PlanStepEx(int p_stepTypeId, ExecutionStateType p_state);
        PlanStepEx(int p_stepTypeId, ExecutionStateType p_state, const PlanStepParameters& p_parameters);
        void InitializeAddressesAux();
        bool IsCurrentStateTimeout(const WorldClock& p_clock);
        virtual void State(ExecutionStateType p_state, RtsGame* pRtsGame, const WorldClock& p_clock);
        virtual void InitializePostConditions() = 0;

    public:
        void Parameters(const PlanStepParameters& p_val) { _params.insert(p_val.begin(), p_val.end()) ; }
        void Copy(IClonable* p_dest);
        void Data(const unsigned p_data) { _data = p_data; }
        void Update(RtsGame* pRtsGame, const WorldClock& p_clock);
        int StepTypeId() const { return _stepTypeId; }
        int Parameter(int p_parameterName) { return _params[(ParameterType)p_parameterName]; }
        int Compare(IComparable* p_rhs) { return !Equals((PlanStepEx*)p_rhs); }
        bool Equals(PlanStepEx* p_planStep);
        const PlanStepParameters& Parameters() const { return _params; }
        unsigned Data() const { return _data; }
        virtual void HandleMessage(RtsGame *pRtsGame, Message* p_msg, bool& p_consumed) {}
        virtual void InitializeConditions();
        virtual bool SuccessConditionsSatisfied(RtsGame* pRtsGame) = 0;
        virtual void UpdateAux(RtsGame* pRtsGame, const WorldClock& p_clock) = 0;
        virtual void Reset(RtsGame* pRtsGame, const WorldClock& p_clock) = 0;
        virtual std::string ToString() const;
        virtual ~PlanStepEx() {}
        PlanStepParameters& Parameters() { return _params; }
        ExecutionStateType State() const { return _state; }
        StepLevelType LevelType() const { return _stepLevelType; }
        IClonable* Clone();
    };
}

#endif // PLANSTEPEX_H
