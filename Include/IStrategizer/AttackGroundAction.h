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
		AttackGroundAction(const PlanStepParameters& p_parameters);
		bool		PreconditionsSatisfied();
		bool		AliveConditionsSatisfied();
		bool		SuccessConditionsSatisfied();

	protected:
		bool		ExecuteAux(const WorldClock& p_clock );
		void		InitializeAddressesAux() ;
		void		HandleMessage(Message* p_pMsg, bool& p_consumed);
		void		InitializePreConditions();
		void		InitializeAliveConditions();
		void		InitializeSuccessConditions();
		void		InitializePostConditions();

	private:
		////> type=int
		TID			 _attackerId;
		
		////> type=CellFeature*
		CellFeature	*_targetCell;
	};
}

#endif	// ATTACKGROUNDACTION_H