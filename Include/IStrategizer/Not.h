///> [Serializable]
#ifndef NOT_H
#define NOT_H

#include <cassert>
#include "CompositeExpression.h"

namespace IStrategizer
{
    ///> class=Not
    ///> parent=CompositeExpression
    class Not : public CompositeExpression
    {
        OBJECT_SERIALIZABLE(Not);

    public:
        Not() {}
        Not(Expression* p_expression) : CompositeExpression(p_expression) {}
        bool Evaluate(RtsGame& p_RtsGame) { assert(_expressions.size() == 1); _isEvaluated = true; return _isSatisfied = !_expressions[0]->Evaluate(p_RtsGame); }
    };
}

#endif // NOT_H