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

///> class=AttackEntityAction
///> parent=Action
class AttackEntityAction : public Action
{

public:
                AttackEntityAction();
	            AttackEntityAction(const PlanStepParameters& p_parameters, CellFeature *p_cellFeature);
    void        Copy(IClonable* p_dest);
	void		Update(unsigned long p_cycles);
	bool		PreconditionsSatisfied();
	bool		AliveConditionsSatisfied();
	bool		SuccessConditionsSatisfied();
	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string      TypeName()  { return "AttackEntityAction"; }
	UserObject* Prototype() { return new AttackEntityAction; }	
	int         TypeSize()  { return sizeof(AttackEntityAction); }
protected:
	//----------------------------------------------------------------------------------------------
	bool		ExecuteAux(unsigned long p_cycles);
	void		OnSucccess(unsigned p_cycles);
	void		OnFailure(unsigned p_cycles);
	void		HandleMessage(Message* p_pMsg, bool& p_consumed);
	void		InitializePreConditions();
	void		InitializeAliveConditions();
	void		InitializeSuccessConditions();
	void		InitializePostConditions();
};

#endif	// ATTACKENTITYACTION_H