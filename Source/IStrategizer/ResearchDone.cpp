#include "ResearchDone.h"

using namespace IStrategizer;

bool ResearchDone::Evaluate(RtsGame* pRtsGame)
{
    int returnValue = g_Assist.ResearchDone((PlayerType)_conditionParameters[PARAM_PlayerId], (ResearchType)_conditionParameters[PARAM_ResearchId]);

    ConditionEx::Evaluate(pRtsGame);

    _isEvaluated = (returnValue == ERR_Success) || (returnValue == ERR_EntityDoesNotExist);
    _isSatisfied = (returnValue == ERR_Success);

    return _isEvaluated && _isSatisfied;
}
