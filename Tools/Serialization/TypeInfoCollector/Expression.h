///> [Serializable]
#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>
#include "EngineMetaData.h"
#include "IClonable.h"

#ifndef USEROBJECT_H
	#include "UserObject.h"
#endif

using namespace std;
using namespace EngineMetaData;
using namespace Serialization;

enum MatchSide
{
    MATCHSIDE_Left,
    MATCHSIDE_Right
};

///> class=Expression
class Expression : public UserObject
{
protected:
    ///> type=int
    ExpressionType      _expressionType;
    ///> type=bool
    bool				_isEvaluated;
    ///> type=bool
    bool				_isSatisfied;

protected:
	void						Initialize(Expression* p_object);

public:
    				            Expression(){}
    				            Expression(ExpressionType p_expressionType) : _expressionType(p_expressionType), _isEvaluated(false), _isSatisfied(false) {}
								Expression(Expression& p_expression) : _expressionType(p_expression._expressionType), 
																	   _isEvaluated(p_expression._isEvaluated), _isSatisfied(p_expression._isSatisfied) {}
	virtual bool                Evaluate() = 0;
    virtual bool				Equals(const Expression* p_rhs) const = 0;
	virtual bool				PartiallyEquals(const Expression* p_rhs, vector<pair<Expression*,Expression*>>& p_matchedLeafs) { return PartiallyEqualsAux(p_rhs, MATCHSIDE_Left, p_matchedLeafs); }
	virtual bool				PartiallyEqualsAux(const Expression* p_rhs, MatchSide p_anchor, vector<pair<Expression*,Expression*>>& p_matchedLeafs) const = 0;
    inline ExpressionType       ExpressionType() const { return _expressionType; }
	inline bool			        IsEvaluated() const { return _isEvaluated; }
	inline void			        IsEvaluated(bool val) { _isEvaluated = val; }
	inline void					IsSatisfied(bool val) { _isSatisfied = val; }
	inline bool			        IsSatisfied() const { return _isSatisfied; }
	inline bool					operator==(Expression* exp) const { Equals(exp); }
	virtual void				ResetSatisfaction() { _isSatisfied = false; }
	virtual	Expression*			Clone() = 0;
    //----------------------------------------------------------------------------------------------------------------------------------------------------	
    // Serialization
protected:
    void            InitializeAddressesAux();
    //----------------------------------------------------------------------------------------------------------------------------------------------------	
};

#endif  // EXPRESSION_H