///> [Serializable]
#ifndef AND_H
#define AND_H

#include "CompositeExpression.h"

namespace IStrategizer
{
    ///> class=And
    ///> parent=CompositeExpression
    class And : public CompositeExpression
    {
        OBJECT_SERIALIZABLE(And);

    protected:
        void And::InitializeAddressesAux();

    public:
        And() {}
        And(const std::vector<Expression*>& p_expressions) : CompositeExpression(p_expressions) {}
        bool Evaluate(RtsGame* pRtsGame);

    };
}

#endif // AND_H
