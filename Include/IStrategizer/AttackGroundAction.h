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
}
using namespace IStrategizer;
///> class=AttackGroundAction
///> parent=Action
class AttackGroundAction : public Action
{
	////> type=CellFeature*
	CellFeature	*_targetCell;

public:
                AttackGroundAction();
				AttackGroundAction(const PlanStepParameters& p_parameters, CellFeature* p_targetCell);
	bool		PreconditionsSatisfied();
	bool		AliveConditionsSatisfied();
	bool		SuccessConditionsSatisfied();
	//----------------------------------------------------------------------------------------------
	// Serialization
public:
	string      TypeName()  { return "AttackGroundAction"; }
    int         TypeSize()  { return sizeof(AttackGroundAction); }
    UserObject* Prototype() { return new AttackGroundAction; }	
	//----------------------------------------------------------------------------------------------
	// Serialization
protected:
	void		InitializeAddressesAux() ;
	//----------------------------------------------------------------------------------------------
	void		OnSucccess(unsigned p_cycles);
	void		OnFailure(unsigned p_cycles);
	bool		ExecuteAux(unsigned long p_cycles );
	void		HandleMessage(Message* p_pMsg, bool& p_consumed);
	void		InitializePreConditions();
	void		InitializeAliveConditions();
	void		InitializeSuccessConditions();
	void		InitializePostConditions();
};


#endif	// ATTACKGROUNDACTION_H