#include "ResearchDone.h"

using namespace IStrategizer;

bool ResearchDone::Evaluate()
{
    int returnValue = EngineAssist::Instance(g_Game).ResearchDone((PlayerType)_conditionParameters[PARAM_PlayerId], (ResearchType)_conditionParameters[PARAM_UpgradeId]);

    ConditionEx::Evaluate();

    _isEvaluated = (returnValue == ERR_Success) || (returnValue == ERR_EntityDoesNotExist);
    _isSatisfied = (returnValue == ERR_Success);

    return _isEvaluated && _isSatisfied;
}
