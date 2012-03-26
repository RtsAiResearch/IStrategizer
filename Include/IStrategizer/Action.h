///> [Serializable]
#ifndef ACTION_H
#define ACTION_H

#include <map>
#include <vector>
#include "PlanStepEx.h"
#include "ErrorCode.h"
#include "MetaData.h"

using namespace std;
using namespace MetaData;

class CompositeExpression;

///> class=Action
///> parent=PlanStepEx
class Action : public PlanStepEx
{
protected:
	CompositeExpression*	_preCondition;
	CompositeExpression*	_aliveCondition;

	Action(ActionType p_actionType, unsigned p_maxPrepTime = 0, unsigned p_maxExecTrialTime = 0, unsigned p_maxExecTime = 0);
	Action(ActionType p_actionType, const PlanStepParameters& p_parameters, unsigned p_maxPrepTime = 0,  unsigned p_maxExecTrialTime = 0, unsigned p_maxExecTime = 0);

public:
	void					UpdateAux(unsigned p_cycles);
	void					Reset(unsigned p_cycles);

	virtual int				Execute(unsigned p_cycles = 0);
	int						Type() const { return PlanStepEx::_stepTypeId; }
	int						PrepareForExecution(unsigned p_cyles = 0);
	void					InitializeConditions();
	virtual bool			AliveConditionsSatisfied() { return _aliveCondition->Evaluate(); }
	virtual bool			PreconditionsSatisfied() { return _preCondition->Evaluate(); }
	void                    Copy(IClonable* p_dest);
	//----------------------------------------------------------------------------------------------
	// Serialization
protected:
	//----------------------------------------------------------------------------------------------
	virtual int				ExecuteAux(unsigned long p_cycles) = 0;
	virtual void			InitializePreConditions() {}
	virtual void			InitializeAliveConditions() {}
};


#endif