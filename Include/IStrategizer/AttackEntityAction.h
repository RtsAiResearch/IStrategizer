///> [Serializable]
#ifndef ATTACKENTITYACTION_H
#define ATTACKENTITYACTION_H

#include "Action.h"
#include "CellFeature.h"

#ifndef CHECKPOSITIONFILTERCOUNT_H
#include "CheckPositionFilterCount.h"
#endif
#ifndef AND_H
#include "And.h"
#endif

namespace IStrategizer
{
	///> class=AttackEntityAction
	///> parent=Action
	class AttackEntityAction : public Action
	{
		OBJECT_SERIALIZABLE(AttackEntityAction);

	public:
		AttackEntityAction();
		AttackEntityAction(const PlanStepParameters& p_parameters, CellFeature *p_cellFeature);
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

#endif	// ATTACKENTITYACTION_H