///> [Serializable]
#ifndef TRAINACTION_H
#define TRAINACTION_H

#include "Action.h"
#ifndef AND_H
#include "And.h"
#endif
#ifndef VECTOR2_H
#include "Vector2.h"
#endif

namespace IStrategizer
{
	class CellFeature;

	///> class=TrainAction
	///> parent=Action
	class TrainAction : public Action
	{
		OBJECT_SERIALIZABLE(TrainAction);

	public:
		TrainAction();
		TrainAction(const PlanStepParameters& p_parameters);
		bool		PreconditionsSatisfied();
		bool		AliveConditionsSatisfied();
		bool		SuccessConditionsSatisfied();

	protected:
		bool		ExecuteAux(const WorldClock& p_clock );
		void		HandleMessage(Message* p_pMsg, bool& p_consumed);

	private:
		TID		_trainerId;
		TID		_traineeId;
		bool	_trainStarted;
	};
}

#endif	// TRAINACTION_H