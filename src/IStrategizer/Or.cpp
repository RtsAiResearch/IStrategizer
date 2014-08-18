#include "Or.h"
#include "ObjectFactory.h"

using namespace IStrategizer;
using namespace std;

DECL_SERIALIZABLE(Or);

bool Or::Evaluate(RtsGame& game)
{
    if(_shortCircuit)
    {
        for(vector<Expression*>::iterator itr = _expressions.begin();
            itr != _expressions.end();
            itr++)
        {
            (*itr)->Evaluate(game);
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
            (*itr)->Evaluate(game);
            _isEvaluated    &= (*itr)->IsEvaluated();
            _isSatisfied    |= (*itr)->IsSatisfied();
        }
    }

    return _isSatisfied && _isEvaluated;
}
