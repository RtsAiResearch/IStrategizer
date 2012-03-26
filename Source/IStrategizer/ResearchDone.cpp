#include "ResearchDone.h"

bool ResearchDone::Evaluate()
{
	int returnValue = g_Assist.ResearchDone((PlayerType)_conditionParameters[PARAM_PlayerId], (ResearchType)_conditionParameters[PARAM_UpgradeId]);

	ConditionEx::Evaluate();

	_isEvaluated = (returnValue == ERR_Success) || (returnValue == ERR_EntityDoesNotExist);
	_isSatisfied = (returnValue == ERR_Success);

	return _isEvaluated && _isSatisfied;
}
