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
}

///> class=TrainAction
///> parent=Action
class TrainAction : public Action
{
public:
	OBJECT_SERIALIZABLE(TrainAction);

	TrainAction();
	TrainAction(const PlanStepParameters& p_parameters);
	bool		PreconditionsSatisfied();
	bool		AliveConditionsSatisfied();
	bool		SuccessConditionsSatisfied();

protected:
	bool		ExecuteAux(unsigned long p_cycles );
	void		HandleMessage(Message* p_pMsg, bool& p_consumed);

private:
	TID		_trainerId;
	bool	_trained;
};
#endif	// TRAINACTION_H