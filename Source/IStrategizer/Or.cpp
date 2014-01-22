#include "Or.h"

using namespace IStrategizer;

bool Or::Evaluate(RtsGame& p_RtsGame)
{
    if(_shortCircuit)
    {
        for(vector<Expression*>::iterator itr = _expressions.begin();
            itr != _expressions.end();
            itr++)
        {
            (*itr)->Evaluate(p_RtsGame);
            _isEvaluated = (*itr)->IsEvaluated();
            _isSatisfied = (*itr)->IsSatisfied();
           
            //short circuit
            if(_isEvaluated && _isSatisfied)
            {
                break;
            }
        }
    }
    else
    {   
        _isEvaluated = true;
        _isSatisfied = false;
        for(vector<Expression*>::iterator itr = _expressions.begin();
            itr != _expressions.end();
            itr++)
        {
            (*itr)->Evaluate(p_RtsGame);
            _isEvaluated    &= (*itr)->IsEvaluated();
            _isSatisfied    |= (*itr)->IsSatisfied();
        }
    }

    return _isSatisfied && _isEvaluated;
}
