#ifndef REVISER_H
#include "Reviser.h"
#endif
#ifndef CASEEX_H
#include "CaseEx.h"
#endif
#include <cstdio>

using namespace OLCBP;

void Reviser::Revise(CaseEx* p_case, bool p_succeeded)
{
	printf("Revising case: %s\n", p_case->Goal()->ToString());

	p_case->TrialCount(p_case->TrialCount() + 1);
	p_case->SuccessCount(p_case->SuccessCount() + (p_succeeded ? 1 : 0));
}