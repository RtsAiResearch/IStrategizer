///> [Serializable]
#ifndef COMPOSITEEXPRESSION_H
#define COMPOSITEEXPRESSION_H

#include "Expression.h"

#ifndef SVECTOR_H
	#include "SVector.h"
#endif
using namespace Serialization;

///> class=CompositeExpression
///> parent=Expression
class CompositeExpression : public Expression
{
protected:
    ///> type=vector(Expression*)
    SVector<Expression*> _expressions;
    ///> type=bool
    bool                _shortCircuit;

    Expression*                 GetTermAux(int p_currentIndex, int p_targetIndex);
    void				        Initialize(CompositeExpression* p_object);
public:
                                CompositeExpression() {}
                                CompositeExpression(const vector<Expression*>& p_expressions) : Expression(EXPRESSION_Composite), _expressions(p_expressions), _shortCircuit(true) {}
                                CompositeExpression(Expression* p_expression) : Expression(EXPRESSION_Composite), _shortCircuit(true) { _expressions.push_back(p_expression); }
    bool                        AddExpression(Expression* p_expression);
    bool                        RemoveExpression(Expression* p_expression);
    bool				        Equals(const Expression* p_rhs) const;
    bool				        PartiallyEqualsAux(const Expression* p_rhs, MatchSide p_anchor, vector<pair<Expression*,Expression*>>& p_matchedLeafs) const;
	bool                        ShortCircuit() const { return _shortCircuit; }
    void                        ShortCircuit(bool val) { _shortCircuit = val; }
    const vector<Expression*>&  Expressions() const { return _expressions; }
    Expression*                 operator[](int p_index);
    Expression*                 At(int p_index);
    CompositeExpression*        Clone() = 0;
    //----------------------------------------------------------------------------------------------
    // Serialization
protected:
    void                        InitializeAddressesAux();
    //----------------------------------------------------------------------------------------------------------------------------------------------------	
};

#endif // _H