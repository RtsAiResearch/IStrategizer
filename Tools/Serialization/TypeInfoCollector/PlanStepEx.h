///> [Serializable]
#ifndef PLANSTEPEX_H
#define PLANSTEPEX_H

#include <map>
#include <vector>
#include <string>
#include "EngineData.h"


#ifndef COMPOSITEEXPRESSION_H
	#include "CompositeExpression.h"
#endif

#ifndef ICOMPARABLE_H
    #include "IComparable.h"
#endif

#ifndef USEROBJECT_H
    #include "UserObject.h"
#endif

#ifndef ICLONABLE_H
    #include "IClonable.h"
#endif

using namespace std;
using namespace EngineMetaData;

class ConditionEx;

///> class=PlanStepEx
class PlanStepEx : public UserObject, public IComparable, public IClonable
{
protected:
    ///> type=int
	int						_stepTypeId;
	ExecutionStateType		_state;
    ///> type=PlanStepParameters
	PlanStepParameters		_parameters;
    ///> type=bool
	bool					_isGoal;
    ///> type=CompositeExpression*
	CompositeExpression*	_aliveCondition;
    ///> type=CompositeExpression*
    CompositeExpression*	_successCondition;
    ///> type=CompositeExpression*
    CompositeExpression*	_postCondition;
    ///> type=int
	StepLevelType			_stepLevelType;

	virtual void	Initialize(PlanStepEx* p_object);

protected:
	inline			PlanStepEx() : _aliveCondition(NULL), _successCondition(NULL), _postCondition(NULL) {}

public:
								PlanStepEx(int p_stepTypeId, ExecutionStateType p_state, const PlanStepParameters& p_parameters, StepLevelType p_stepLevelType);
	inline int					StepTypeId() const { return _stepTypeId; }
	inline PlanStepParameters	Parameters() const { return _parameters; }
	inline void					Parameters(const PlanStepParameters& p_val) { _parameters.insert(p_val.begin(), p_val.end()) ; }
	inline int					Parameter(int p_parameterName) { return _parameters[(ParameterType)p_parameterName]; }
	inline bool					IsGoal() const {return _isGoal; };
	inline CompositeExpression* SuccessConditions() const { return _successCondition; }
	inline CompositeExpression* AliveConditions() const { return _aliveCondition; }
	inline CompositeExpression*	PostConditions() const { return _postCondition; }
	inline ExecutionStateType	State() const { return _state; }
	inline ExecutionStateType	State(ExecutionStateType p_state) { return _state = p_state; }
	inline StepLevelType		LevelType() const { return _stepLevelType; }
	bool						Equals(PlanStepEx* p_planStep);
	int							Compare(IComparable* p_rhs) { return !Equals((PlanStepEx*)p_rhs); }
	virtual PlanStepEx*			Clone() = 0;
    //----------------------------------------------------------------------------------------------------------------------------------------------------	
    // Serialization
protected:
    void                        InitializeAddressesAux();
    //----------------------------------------------------------------------------------------------------------------------------------------------------	
};

#endif	// PLANSTEPEX_H