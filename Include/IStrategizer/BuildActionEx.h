///> [Serializable]
#ifndef BUILDACTIONEX_H
#define BUILDACTIONEX_H

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
///> class=BuildActionEx
///> parent=Action
class BuildActionEx : public Action
{
	TID				_buildingId;
	TID				_builderId;
	Vector2			_buildPosition;
	bool			_buildStarted;

public:
	BuildActionEx();
	BuildActionEx(const PlanStepParameters& p_parameters);
	bool		PreconditionsSatisfied();
	bool		AliveConditionsSatisfied();
	bool		SuccessConditionsSatisfied();
	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string      TypeName()  { return "BuildActionEx"; }
	int         TypeSize()  { return sizeof(BuildActionEx); }
	UserObject* Prototype() { return new BuildActionEx; }	
protected:
	//----------------------------------------------------------------------------------------------
	int			ExecuteAux(unsigned long p_cycles );
	void		HandleMessage(Message* p_pMsg, bool& p_consumed);
};
#endif	// BUILDACTIONEX_H