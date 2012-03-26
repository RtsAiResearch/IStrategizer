///> [Serializable]
#ifndef TRAINFORCEGOAL_H
#define TRAINFORCEGOAL_H

#include "GoalEx.h"

///> class=TrainForceGoal
///> parent=GoalEx
class TrainForceGoal : public GoalEx
{
public:
                TrainForceGoal();
                TrainForceGoal(const PlanStepParameters& p_parameters);
    //----------------------------------------------------------------------------------------------
    // Serialization
    string      TypeName()  { return "TrainForceGoal"; }
    int         TypeSize()  { return sizeof(TrainForceGoal); }
    UserObject* Prototype() { return new TrainForceGoal; }
    //----------------------------------------------------------------------------------------------
protected:
	void InitializeSuccessConditions();
	void InitializePostConditions();
};

#endif	// TRAINFORCEGOAL_H