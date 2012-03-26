///> [Serializable]
#ifndef RESEARCHACTION_H
#define RESEARCHACTION_H

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
///> class=ResearchAction
///> parent=Action
class ResearchAction : public Action
{
	TID				_researcherId;

public:
	ResearchAction();
	ResearchAction(const PlanStepParameters& p_parameters);
	bool		PreconditionsSatisfied();
	bool		AliveConditionsSatisfied();
	bool		SuccessConditionsSatisfied();
	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string      TypeName()  { return "ResearchAction"; }
	int         TypeSize()  { return sizeof(ResearchAction); }
	UserObject* Prototype() { return new ResearchAction; }	
protected:
	//----------------------------------------------------------------------------------------------
	int			ExecuteAux(unsigned long p_cycles );
};
#endif	// RESEARCHACTION_H