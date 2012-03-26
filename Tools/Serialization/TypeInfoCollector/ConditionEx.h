///> [Serializable]
#ifndef CONDITIONEX_H
#define CONDITIONEX_H

#include <map>
#include "EngineMetaData.h"
#include "ErrorCode.h"
#include "EngineAPI.h"
#include "Comparer.h"
#include "Expression.h"

using namespace std;
using namespace EngineMetaData;

///> class=ConditionEx
///> parent=Expression
class ConditionEx : public Expression
{
protected:
    ///> type=int
	ConditionTypeEx				_conditionType;
    ///> type=PlanStepParameters
	PlanStepParameters		    _conditionParameters;
    ///> type=int
	PlayerType					_playerType;
    ///> type=bool
	bool						_consumable;

	inline	ConditionEx(){}
	void	Initialize(ConditionEx* p_object);
	
public:
	inline						ConditionEx(PlayerType p_player, ConditionTypeEx p_conditionType, bool p_consumable = true) : Expression(EXPRESSION_Leaf), _conditionType(p_conditionType), _playerType(p_player), _consumable(p_consumable) { _conditionParameters[PARAM_PlayerId] = p_player; }
	inline int					Type() const { return _conditionType; } 
    inline int					Parameter(int p_paramId) { return _conditionParameters[(ParameterType)p_paramId]; }
    inline void					Parameter(int p_paramId, int p_newValue) { _conditionParameters[(ParameterType)p_paramId] = p_newValue; }
	bool						PartiallyEqualsAux(const Expression* p_rhs,  MatchSide p_anchor, vector<pair<Expression*,Expression*>>& p_matchedLeafs) const;
    bool						Evaluate() { _isEvaluated = _isSatisfied = true; return _isEvaluated && _isSatisfied; }
    bool		                Equals(const Expression* p_rhs) const;
    inline PlanStepParameters   Parameters() const { return _conditionParameters; }
	virtual ConditionEx*		Clone() = 0;
	inline bool					Consumable() const { return _consumable; }
    //----------------------------------------------------------------------------------------------------------------------------------------------------	
    // Serialization
protected:
    void            InitializeAddressesAux();
    //----------------------------------------------------------------------------------------------------------------------------------------------------	
};

#endif  // CONDITIONEX_H