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
#ifndef MAPAREA_H
#include "MapArea.h"
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
public:
	OBJECT_SERIALIZABLE(BuildActionEx);

	BuildActionEx();
	BuildActionEx(const PlanStepParameters& p_parameters);
	bool		PreconditionsSatisfied();
	bool		AliveConditionsSatisfied();
	bool		SuccessConditionsSatisfied();

protected:
	void		OnSucccess(unsigned p_cycles);
	void		OnFailure(unsigned p_cycles);
	bool		ExecuteAux(unsigned long p_cycles );
	void		HandleMessage(Message* p_pMsg, bool& p_consumed);

private:
	TID						_buildingId;
	TID						_builderId;
	IStrategizer::MapArea	_buildArea;
	bool					_buildStarted;
	bool					_buildIssued;
};
#endif	// BUILDACTIONEX_H