#ifndef EXPRESSION_H
	#include "Expression.h"
#endif

void Expression::InitializeAddressesAux()
{
    AddMemberAddress(3,
        &_expressionType,
        &_isEvaluated,
        &_isSatisfied);
}
//----------------------------------------------------------------------------------------------
IClonable* Expression::Clone()
{
    Expression* clone = static_cast<Expression*>(Prototype());
    Copy(clone);

    return clone;
}
//----------------------------------------------------------------------------------------------
void Expression::Copy(IClonable* p_dest)
{
    Expression* m_dest = static_cast<Expression*>(p_dest);

	m_dest->_expressionType   = _expressionType;
	m_dest->_isEvaluated      = _isEvaluated;
	m_dest->_isSatisfied      = _isSatisfied;
}
