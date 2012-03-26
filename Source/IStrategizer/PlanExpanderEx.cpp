#ifndef PLANEXPANDEREX_H
#include "PlanExpanderEx.h"
#endif

#ifndef CASEBASEDREASONEREX_H
#include "CaseBasedReasonerEx.h"
#endif

PlanExpanderEx::PlanExpanderEx(CaseBasedReasonerEx* p_caseBasedReasoner)
{
	_caseBasedReasoner = p_caseBasedReasoner;
}
//----------------------------------------------------------------------------------------------
vector<ActionEx*> PlanExpanderEx::GetReadyActions()
{
	vector<ActionEx*> readyActions;
	return readyActions;
}
