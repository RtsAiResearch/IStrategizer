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
		GatherResourceAction(const PlanStepParameters& p_parameters);
		bool		PreconditionsSatisfied();
		bool		AliveConditionsSatisfied();
		bool		SuccessConditionsSatisfied();

	protected:
		void		OnSucccess(const WorldClock& p_clock);
		void		OnFailure(const WorldClock& p_clock);
		bool		ExecuteAux(const WorldClock& p_clock);
		void		HandleMessage(Message* p_pMsg, bool& p_consumed);

	private:
		TID			_gathererId;
		TID			_resourceId;
		int			_resourceAmount;
		bool		_gatherIssued;
		bool		_gatherStarted;
	};
}

#endif	// GATHERRESOURCEACTION_H