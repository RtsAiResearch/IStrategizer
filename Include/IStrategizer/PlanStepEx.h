///> [Serializable]
#ifndef PLANSTEPEX_H
#define PLANSTEPEX_H

#include <map>
#include <vector>
#include <string>

#ifndef COMPOSITEEXPRESSION_H
#include "CompositeExpression.h"
#endif

#ifndef ENGINEDATA_H
#include "EngineData.h"
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

#include "CellFeature.h"

using namespace std;
using namespace MetaData;
using namespace IStrategizer;

class ConditionEx;
class Message;

///> class=PlanStepEx
class PlanStepEx : public UserObject, public IComparable, public IClonable
{
private:
	ExecutionStateType		_state;

protected:
	///> type=PlanStepParameters
	PlanStepParameters		_params;
	int						_stepTypeId;
	unsigned                _data;
	StepLevelType			_stepLevelType;
	CompositeExpression*	_successCondition;
	CompositeExpression*	_postCondition;
	CellFeature*			_cellFeature;
	string					_stepDescription;
	unsigned				_stateStartTime[COUNT(ExecutionStateType)];
	unsigned				_stateTimeout[COUNT(ExecutionStateType)];
	bool					_firstUpdate;

	void					State(ExecutionStateType p_state, unsigned p_cycles);
	bool					IsCurrentStateTimeout(unsigned p_cycles);

	PlanStepEx(int p_stepTypeId, ExecutionStateType p_state);
	PlanStepEx(int p_stepTypeId, ExecutionStateType p_state, const PlanStepParameters& p_parameters);
public:

	virtual						~PlanStepEx() {}
	int							StepTypeId() const { return _stepTypeId; }
	PlanStepParameters&			Parameters() { return _params; }
	const PlanStepParameters&	Parameters() const { return _params; }
	void						Parameters(const PlanStepParameters& p_val) { _params.insert(p_val.begin(), p_val.end()) ; }
	int							Parameter(int p_parameterName) { return _params[(ParameterType)p_parameterName]; }
	ExecutionStateType			State() const { return _state; }
	StepLevelType				LevelType() const { return _stepLevelType; }
	unsigned					Data() const { return _data; }
	void						Data(const unsigned p_data) { _data = p_data; }
	bool						Equals(PlanStepEx* p_planStep);
	int							Compare(IComparable* p_rhs) { return !Equals((PlanStepEx*)p_rhs); }
	IClonable*                  Clone();
	void                        Copy(IClonable* p_dest);
	virtual void				HandleMessage(Message* p_msg, bool& p_consumed) {}
	virtual void				InitializeConditions();
	virtual bool				SuccessConditionsSatisfied() { return _successCondition->Evaluate(); }
	void						Update(unsigned p_cycles);
	virtual void				UpdateAux(unsigned p_cycles) = 0;
	virtual void				Reset(unsigned p_cycles) = 0;
	virtual const char*			ToString();
	//----------------------------------------------------------------------------------------------------------------------------------------------------	
	// Serialization
protected:
	void                        InitializeAddressesAux();
	//----------------------------------------------------------------------------------------------------------------------------------------------------	
	virtual void				InitializeSuccessConditions() {}
	virtual void				InitializePostConditions() {}
};
#endif	// PLANSTEPEX_H