///> [Serializable]
#ifndef CONDITIONEX_H
#define CONDITIONEX_H

#include <map>
#include "MetaData.h"
#include "ErrorCode.h"
#include "EngineAssist.h"
#include "Comparer.h"
#include "Expression.h"

using namespace std;
using namespace MetaData;

///> class=ConditionEx
///> parent=Expression
class ConditionEx : public Expression
{
protected:
    ///> type=int
	ConditionType				_conditionType;
    ///> type=PlanStepParameters
	PlanStepParameters		    _conditionParameters;
    ///> type=int
	PlayerType					_playerType;

	inline	ConditionEx(){}
	
public:
	inline						ConditionEx(PlayerType p_player, ConditionType p_conditionType) : Expression(EXPRESSION_Leaf), _conditionType(p_conditionType), _playerType(p_player) { _conditionParameters[PARAM_PlayerId] = p_player; }
	inline int					Type() const { return _conditionType; } 
    inline int					Parameter(int p_paramId) { return _conditionParameters[(ParameterType)p_paramId]; }
    inline void					Parameter(int p_paramId, int p_newValue) { _conditionParameters[(ParameterType)p_paramId] = p_newValue; }
	bool						PartiallyEqualsAux(const Expression* p_rhs,  MatchSide p_anchor, vector<pair<Expression*,Expression*>>& p_matchedLeafs) const;
    bool						Evaluate() { _isEvaluated = _isSatisfied = true; return _isEvaluated && _isSatisfied; }
    bool		                Equals(const Expression* p_rhs) const;
    inline PlanStepParameters   Parameters() const { return _conditionParameters; }
	virtual bool				Consume(int p_amount) = 0;
    void                        Copy(IClonable* p_dest);
	inline bool					ContainsParameter(ParameterType p_parameter) { return (_conditionParameters.find(p_parameter) == _conditionParameters.end()); }
    //----------------------------------------------------------------------------------------------------------------------------------------------------	
    // Serialization
protected:
    void            InitializeAddressesAux();
    //----------------------------------------------------------------------------------------------------------------------------------------------------	
};

#endif  // CONDITIONEX_H