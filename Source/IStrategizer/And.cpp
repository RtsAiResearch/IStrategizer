#include "And.h"

using namespace std;
using namespace IStrategizer;

void And::InitializeAddressesAux()
{
    CompositeExpression::InitializeAddressesAux();
}
//----------------------------------------------------------------------------------------------
bool And::Evaluate(RtsGame& p_RtsGame)
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
            (*itr)->Evaluate(p_RtsGame);
            _isEvaluated    &= (*itr)->IsEvaluated();
            _isSatisfied    &= (*itr)->IsSatisfied();
        }
    }

    return _isSatisfied && _isEvaluated;
}
