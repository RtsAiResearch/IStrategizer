///> [Serializable]
#ifndef MOVEACTION_H
#define MOVEACTION_H

#include "Action.h"
#ifndef VECTOR2_H
#include "Vector2.h"
#endif

namespace IStrategizer
{
	class CellFeature;

	///> class=MoveAction
	///> parent=Action
	class MoveAction : public Action
	{
		OBJECT_SERIALIZABLE(MoveAction);

	public:
		MoveAction();
		MoveAction(const PlanStepParameters& p_parameters);
		bool		PreconditionsSatisfied();
		bool		AliveConditionsSatisfied();
		bool		SuccessConditionsSatisfied();

	protected:
		void		OnSucccess(const WorldClock& p_clock);
		void		OnFailure(const WorldClock& p_clock);
		bool		ExecuteAux(const WorldClock& p_clock);
		void		HandleMessage(Message* p_pMsg, bool& p_consumed);

	private:
		TID			_entityId;
		Vector2		_position;
	};
}

#endif	// MOVEACTION_H