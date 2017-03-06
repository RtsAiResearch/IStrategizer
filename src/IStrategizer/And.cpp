#include "And.h"
#include "ObjectFactory.h"

using namespace std;
using namespace IStrategizer;

DECL_SERIALIZABLE(And);

bool And::Evaluate(RtsGame& game)
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
            if(!_isEvaluated || !_isSatisfied)
            {
                break;
            }
        }
    }
    else
    {
        _isEvaluated = true;
        _isSatisfied = true;
        for(vector<Expression*>::iterator itr = _expressions.begin();
            itr != _expressions.end();
            itr++)
        {
            (*itr)->Evaluate(game);
            _isEvaluated    &= (*itr)->IsEvaluated();
            _isSatisfied    &= (*itr)->IsSatisfied();
        }
    }

    return _isSatisfied && _isEvaluated;
}
