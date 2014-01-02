///> [Serializable]
#ifndef GATHERRESOURCEACTION_H
#define GATHERRESOURCEACTION_H

#include "Action.h"

namespace IStrategizer
{
	///> class=GatherResourceAction
	///> parent=Action
	class GatherResourceAction : public Action
	{
		OBJECT_SERIALIZABLE(GatherResourceAction);

	public:
		GatherResourceAction();
		GatherResourceAction(const PlanStepParameters& p_parameters, CellFeature *p_cellFeature);
		void        Copy(IClonable* p_dest);
		void		Update(const WorldClock& p_clock);

	protected:
		bool		ExecuteAux(const WorldClock& p_clock);
		void		InitializePreConditions();
		void		InitializeAliveConditions();
		void		InitializeSuccessConditions();
		void		InitializePostConditions();
	};
}

#endif	// GATHERRESOURCEACTION_H