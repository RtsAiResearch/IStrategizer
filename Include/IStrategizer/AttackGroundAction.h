///> [Serializable]
#ifndef ATTACKGROUNDACTION_H
#define ATTACKGROUNDACTION_H

#include "Action.h"
#ifndef AND_H
#include "And.h"
#endif

namespace IStrategizer
{
	class CellFeature;

	///> class=AttackGroundAction
	///> parent=Action
	class AttackGroundAction : public Action
	{
	public:
		OBJECT_SERIALIZABLE(AttackGroundAction);

		AttackGroundAction();
		AttackGroundAction(const PlanStepParameters& p_parameters, CellFeature* p_targetCell);
		////> type=CellFeature*
		CellFeature	*_targetCell;
		//----------------------------------------------------------------------------------------------
		// Serialization
	protected:
		void		InitializeAddressesAux() ;
		//----------------------------------------------------------------------------------------------
		bool		ExecuteAux(unsigned long p_cycles);
		void		InitializePreConditions();
		void		InitializeAliveConditions();
		void		InitializeSuccessConditions();
		void		InitializePostConditions();
	};
}

#endif	// ATTACKGROUNDACTION_H