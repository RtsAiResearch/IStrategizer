#include "EntityObjectExist.h"

using namespace IStrategizer;

bool EntityObjectExist::Evaluate()
{
    if(_conditionParameters.find(PARAM_EntityObjectId) == _conditionParameters.end())
    {
        assert(_entityIdPtr != nullptr);
        _conditionParameters[PARAM_EntityObjectId] = *_entityIdPtr;
    }

    int m_returnValue = EngineAssist::Instance(g_Game).GetObject((PlayerType)_conditionParameters[PARAM_PlayerId], _conditionParameters[PARAM_EntityObjectId]);

    ConditionEx::Evaluate();

    if (m_returnValue == ERR_Success)
    {
        _isEvaluated = true;
        _isSatisfied = true;
    }
    else if(m_returnValue == ERR_EntityDoesNotExist)
    {
        _isEvaluated = true;
        _isSatisfied = false;
    }

    return _isEvaluated && _isSatisfied;
}
