///> [Serializable]
#ifndef AND_H
#define AND_H

#include "CompositeExpression.h"

///> class=And
///> parent=CompositeExpression
class And : public CompositeExpression
{
public:
                And() {}
			    And(const vector<Expression*>& p_expressions) : CompositeExpression(p_expressions) {}
    bool	    Evaluate();
    //----------------------------------------------------------------------------------------------
    // Serialization
    string      TypeName()  { return "And"; }
    int         TypeSize()  { return sizeof(And); }
    UserObject* Prototype() { return new And; }
    //----------------------------------------------------------------------------------------------
};

#endif // AND_H