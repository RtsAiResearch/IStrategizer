///> [Serializable]
#ifndef ACTION_H
#define ACTION_H

#include <map>
#include <vector>
#include "PlanStepEx.h"
#include "MetaData.h"
#include "ExecutionHistory.h"

namespace IStrategizer
{
    class CompositeExpression;
    class ExecutionHistory;

    ///> class=Action
    ///> parent=PlanStepEx
    class Action : public PlanStepEx
    {
		OBJECT_SERIALIZABLE_P(Action, PlanStepEx, &m_history);
	public:
		~Action();
		CompositeExpression* PreCondition() { _ASSERTE(_preCondition); return _preCondition; }
		void InitializeConditions();
		void Copy(IClonable* p_dest);
		bool PreconditionsSatisfied(RtsGame& game);
		ExecutionHistory GetExecutionHistory() const { return m_history; }
		void SetExecutionHistory(ExecutionHistory history) { m_history.AddRange(history); }
		virtual bool AliveConditionsSatisfied(RtsGame& game) = 0;
		unsigned Hash(bool quantified /* = true */) const;
		virtual void Abort(RtsGame &game) { LogInfo("%s is aborting", ToString().c_str()); FreeResources(game); }
		virtual bool Execute(RtsGame& game, const WorldClock& p_clock) = 0;
		virtual void Update(RtsGame& game, const WorldClock& p_clock);
		void SetState(ExecutionStateType p_state, RtsGame& game, const WorldClock& p_clock);

		void Reset(RtsGame& game, const WorldClock& p_clock)
		{
			if (GetState() != ESTATE_NotPrepared)
				SetState(ESTATE_NotPrepared, game, p_clock);
		}

    protected:
		Action(ActionType p_actionType, unsigned p_maxPrepTime = 0, unsigned p_maxExecTime = 0);
        Action(ActionType p_actionType, const PlanStepParameters& p_parameters, unsigned p_maxPrepTime = 0, unsigned p_maxExecTime = 0);
        virtual void InitializePreConditions() = 0;
        virtual void OnSucccess(RtsGame& game, const WorldClock& p_clock) { FreeResources(game); };
        virtual void OnFailure(RtsGame& game, const WorldClock& p_clock) { FreeResources(game); };
        virtual void FreeResources(RtsGame& game) {}

		void SetStateTimeout(ExecutionStateType state, unsigned timeout) 
		{
			LogInfo("Setting %s state=%s new time out to %d", ToString().c_str(), Enums[state], timeout);
			_stateTimeout[INDEX(state, ExecutionStateType)] = timeout; 
		}

		unsigned CurrentStateElapsedTime(const WorldClock& clock) const { return clock.ElapsedGameCycles() - _stateStartTime[INDEX(_state, ExecutionStateType)]; }
		bool IsCurrentStateTimeout(const WorldClock& p_clock);

		CompositeExpression* _preCondition;
		///> type=ExecutionHistory
		ExecutionHistory m_history;
		unsigned _stateStartTime[COUNT(ExecutionStateType)];
		unsigned _stateTimeout[COUNT(ExecutionStateType)];
    };
}

#endif
