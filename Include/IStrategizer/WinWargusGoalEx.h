///> [Serializable]
#ifndef WINWARGUSGOALEX_H
#define WINWARGUSGOALEX_H

#include "GoalEx.h"
#include "CheckEntityClassAttribute.h"

namespace IStrategizer
{
	///> class=WinWargusGoalEx
	///> parent=GoalEx
	class WinWargusGoalEx : public GoalEx
	{
		OBJECT_SERIALIZABLE(WinWargusGoalEx);

	public:
		WinWargusGoalEx();
		WinWargusGoalEx(const PlanStepParameters& p_parameters);
		void InitializeSuccessConditions();
		void InitializePostConditions();
	};
}

#endif	// WINWARGUSGOALEX_H