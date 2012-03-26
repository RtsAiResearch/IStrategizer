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
void Expression::Initialize(Expression* p_object)
{
	p_object->_expressionType = _expressionType;
	p_object->_isEvaluated = _isEvaluated;
	p_object->_isSatisfied = _isSatisfied;
}