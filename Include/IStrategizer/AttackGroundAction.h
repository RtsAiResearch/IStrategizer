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
	
public:
                AttackGroundAction();
				AttackGroundAction(const PlanStepParameters& p_parameters, CellFeature* p_targetCell);
	////> type=CellFeature*
	CellFeature	*_targetCell;
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
	int			ExecuteAux(unsigned long p_cycles);
	void		InitializePreConditions();
	void		InitializeAliveConditions();
	void		InitializeSuccessConditions();
	void		InitializePostConditions();
};


#endif	// ATTACKGROUNDACTION_H