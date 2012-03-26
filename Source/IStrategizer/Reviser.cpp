#ifndef REVISER_H
#include "Reviser.h"
#endif
#ifndef CASEEX_H
#include "CaseEx.h"
#endif

using namespace OLCBP;

void Reviser::Revise(CaseEx* p_case, bool p_succeeded)
{
	p_case->TrialCount(p_case->TrialCount() + 1);
	p_case->SuccessCount(p_case->SuccessCount() + (int)p_succeeded);
}