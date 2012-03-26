///> [Serializable]
#ifndef NOT_H
#define NOT_H

#include <cassert>
#include "CompositeExpression.h"

///> class=Not
///> parent=CompositeExpression
class Not : public CompositeExpression
{
public:
                Not() {}
                Not(Expression* p_expression) : CompositeExpression(p_expression) {}
    bool	    Evaluate() { assert(_expressions.size() == 1); _isEvaluated = true; return _isSatisfied = !_expressions[0]->Evaluate(); }
    //----------------------------------------------------------------------------------------------
    // Serialization
    string      TypeName()  { return "Not"; }
    int         TypeSize()  { return sizeof(Not); }
    UserObject* Prototype() { return new Not; }
    //----------------------------------------------------------------------------------------------
};

#endif // NOT_H