#include "ResearchDone.h"

using namespace IStrategizer;

bool ResearchDone::Evaluate(RtsGame& p_RtsGame)
{
    int returnValue = EngineAssist::Instance(&p_RtsGame).ResearchDone((PlayerType)_conditionParameters[PARAM_PlayerId], (ResearchType)_conditionParameters[PARAM_ResearchId]);

    ConditionEx::Evaluate(p_RtsGame);

    _isEvaluated = (returnValue == ERR_Success) || (returnValue == ERR_EntityDoesNotExist);
    _isSatisfied = (returnValue == ERR_Success);

    return _isEvaluated && _isSatisfied;
}
