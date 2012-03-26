///> [Serializable]
#ifndef ATTACKENEMYGOAL_H
#define ATTACKENEMYGOAL_H

#include "GoalEx.h"
#include "Colony.h"

///> class=AttackEnemyGoal
///> parent=GoalEx
class AttackEnemyGoal : public GoalEx
{
private:
    ForceDescriptionEx _forceDescription;
    Colony             _cachedColony;

public:
                AttackEnemyGoal();
	            AttackEnemyGoal(const PlanStepParameters& p_parameters);
    void		InitSuccessConditions();
    void        Copy(IClonable* p_dest);
    //----------------------------------------------------------------------------------------------
    // Serialization
    string      TypeName()  { return "AttackEnemyGoal"; }
    int         TypeSize()  { return sizeof(AttackEnemyGoal); }
    UserObject* Prototype() { return new AttackEnemyGoal; }
    //----------------------------------------------------------------------------------------------
protected:
	void InitializeSuccessConditions();
	void InitializePostConditions();
};

#endif	// ATTACKENEMYGOAL_H