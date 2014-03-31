///> [Serializable]
#ifndef GATHERRESOURCEACTION_H
#define GATHERRESOURCEACTION_H

#include "Action.h"
#ifndef AND_H
#include "And.h"
#endif
#ifndef VECTOR2_H
#include "Vector2.h"
#endif

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
		bool		AliveConditionsSatisfied(RtsGame* pRtsGame);
		bool		SuccessConditionsSatisfied(RtsGame* pRtsGame);

	protected:
		bool		ExecuteAux(RtsGame* pRtsGame, const WorldClock& p_clock);
		void		InitializePreConditions();
		void		InitializePostConditions();
		void		OnSucccess(RtsGame* pRtsGame, const WorldClock& p_clock);
		void		OnFailure(RtsGame* pRtsGame, const WorldClock& p_clock);
		void		HandleMessage(RtsGame* pRtsGame, Message* p_msg, bool& p_consumed);

	private:
		TID			_gathererId;
		TID			_resourceId;
		int			_resourceAmount;
		bool		_gatherIssued;
		bool		_gatherStarted;
		float		_gatheredAmount;
	};
}

#endif	// GATHERRESOURCEACTION_H