///> [Serializable]
#ifndef BUILDACTION_H
#define BUILDACTION_H

#include "Action.h"
#ifndef AND_H
	#include "And.h"
#endif

namespace IStrategizer
{
	class CellFeature;
}
using namespace IStrategizer;
///> class=BuildAction
///> parent=Action
class BuildAction : public Action
{
public:
	OBJECT_SERIALIZABLE(BuildAction);

                BuildAction();
	            BuildAction(const PlanStepParameters& p_parameters, CellFeature *p_targetCell);
    void        Copy(IClonable* p_dest);

protected:
	bool		ExecuteAux(unsigned long p_cycles );
	void		InitializePreConditions();
	void		InitializeAliveConditions();
	void		InitializeSuccessConditions();
	void		InitializePostConditions();
	void		HandleMessage(Message* p_msg, bool& p_consumed);

private:
	CellFeature*	_targetCell;
	int				_buildingObjectId;
};
#endif	// BUILDACTION_H