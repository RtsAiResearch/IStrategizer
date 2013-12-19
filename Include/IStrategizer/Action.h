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
		CompositeExpression*	_preCondition;
		CompositeExpression*	_aliveCondition;

		Action(ActionType p_actionType, unsigned p_maxPrepTime = 0, unsigned p_maxExecTrialTime = 0, unsigned p_maxExecTime = 0);
		Action(ActionType p_actionType, const PlanStepParameters& p_parameters, unsigned p_maxPrepTime = 0,  unsigned p_maxExecTrialTime = 0, unsigned p_maxExecTime = 0);
		void					State(ExecutionStateType p_state, const WorldClock& p_clock);
		virtual void			OnSucccess(const WorldClock& p_clock) {};
		virtual void			OnFailure(const WorldClock& p_clock) {};

	public:
		void					UpdateAux(const WorldClock& p_clock);
		void					Reset(const WorldClock& p_clock);

		virtual bool			Execute(const WorldClock& p_clock);
		int						Type() const { return PlanStepEx::_stepTypeId; }
		int						PrepareForExecution(const WorldClock& p_clock);
		void					InitializeConditions();
		virtual bool			AliveConditionsSatisfied() { assert(0); return false; }
		virtual bool			PreconditionsSatisfied() { assert(0); return false; }
		void                    Copy(IClonable* p_dest);

	protected:
		virtual bool			ExecuteAux(const WorldClock& p_clock) = 0;
		virtual void			InitializePreConditions() {}
		virtual void			InitializeAliveConditions() {}
	};
}

#endif