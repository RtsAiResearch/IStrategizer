///> [Serializable]
#ifndef WINWARGUSGOALEX_H
#define WINWARGUSGOALEX_H

#include "GoalEx.h"
#include "CheckEntityClassAttribute.h"
using namespace MetaData;

///> class=WinWargusGoalEx
///> parent=GoalEx
class WinWargusGoalEx : public GoalEx
{

public:
	WinWargusGoalEx();
	WinWargusGoalEx(const PlanStepParameters& p_parameters);
	//----------------------------------------------------------------------------------------------
	// Serialization
	string      TypeName()  { return "WinWargusGoalEx"; }
	int         TypeSize()  { return sizeof(WinWargusGoalEx); }
	UserObject* Prototype() { return new WinWargusGoalEx; }
	//----------------------------------------------------------------------------------------------
	void InitializeSuccessConditions();
	void InitializePostConditions();
};

#endif	// WINWARGUSGOALEX_H