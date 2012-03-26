#include "And.h"

bool And::Evaluate()
{
	if(_shortCircuit)
    {
        for(vector<Expression*>::iterator itr = _expressions.begin();
            itr != _expressions.end();
            itr++)
        {
            (*itr)->Evaluate();
            _isEvaluated    = (*itr)->IsEvaluated();
            _isSatisfied    = (*itr)->IsSatisfied();

            //short circuit
            if(!_isEvaluated || !_isSatisfied)
            {
                break;
            }
        }
    }
    else
    {   
        _isEvaluated    = true;
        _isSatisfied    = true;
        for(vector<Expression*>::iterator itr = _expressions.begin();
            itr != _expressions.end();
            itr++)
        {
            (*itr)->Evaluate();
            _isEvaluated    &= (*itr)->IsEvaluated();
            _isSatisfied    &= (*itr)->IsSatisfied();
        }
    }

    return _isSatisfied && _isEvaluated;
}
