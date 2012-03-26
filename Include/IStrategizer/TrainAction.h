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
using namespace IStrategizer;
///> class=TrainAction
///> parent=Action
class TrainAction : public Action
{
	TID				_trainerId;
	bool			_trained;

public:
	TrainAction();
	TrainAction(const PlanStepParameters& p_parameters);
	bool		PreconditionsSatisfied();
	bool		AliveConditionsSatisfied();
	bool		SuccessConditionsSatisfied();
	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string      TypeName()  { return "TrainAction"; }
	int         TypeSize()  { return sizeof(TrainAction); }
	UserObject* Prototype() { return new TrainAction; }	
protected:
	//----------------------------------------------------------------------------------------------
	int			ExecuteAux(unsigned long p_cycles );
	void		HandleMessage(Message* p_pMsg, bool& p_consumed);
};
#endif	// TRAINACTION_H