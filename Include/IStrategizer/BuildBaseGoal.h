///> [Serializable]
#ifndef BUILDBASEGOAL_H
#define BUILDBASEGOAL_H

#include "GoalEx.h"
#include "EntityClassExist.h"

///> class=BuildBaseGoal
///> parent=GoalEx
class BuildBaseGoal : public GoalEx
{ 
public:
                BuildBaseGoal();
                BuildBaseGoal(const PlanStepParameters& p_parameters);
    //----------------------------------------------------------------------------------------------
    // Serialization
    string      TypeName()  { return "BuildBaseGoal"; }
    int         TypeSize()  { return sizeof(BuildBaseGoal); }
    UserObject* Prototype() { return new BuildBaseGoal; }
    //----------------------------------------------------------------------------------------------
protected:
	void InitializeSuccessConditions();
	void InitializePostConditions();
};

#endif	// BUILDBASEGOAL_H