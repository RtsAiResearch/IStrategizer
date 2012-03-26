///> [Serializable]
#ifndef OR_H
#define OR_H

#include "CompositeExpression.h"

///> class=Or
///> parent=CompositeExpression
class Or : public CompositeExpression
{
public:
                Or() {}
                Or(const vector<Expression*>& p_expressions) : CompositeExpression(p_expressions) {}
    bool        Evaluate();
    //----------------------------------------------------------------------------------------------
    // Serialization
    string      TypeName()  { return "Or"; }
    int         TypeSize()  { return sizeof(Or); }
    UserObject* Prototype() { return new Or; }
    //----------------------------------------------------------------------------------------------
};

#endif // OR_H