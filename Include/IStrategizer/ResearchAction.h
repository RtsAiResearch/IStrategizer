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

public:
	OBJECT_SERIALIZABLE(ResearchAction);

	ResearchAction();
	ResearchAction(const PlanStepParameters& p_parameters);
	bool		PreconditionsSatisfied();
	bool		AliveConditionsSatisfied();
	bool		SuccessConditionsSatisfied();

protected:
	bool		ExecuteAux(unsigned long p_cycles );

private:
	TID		_researcherId;
};
#endif	// RESEARCHACTION_H