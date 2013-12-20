#ifndef REVISER_H
#include "Reviser.h"
#endif
#ifndef CASEEX_H
#include "CaseEx.h"
#endif
#include <cstdio>
#include "Logger.h"

using namespace IStrategizer;

void Reviser::Revise(CaseEx* p_case, bool p_succeeded)
{
	LogWarning("Pretending to revise case %s, but case revision is not functional actually", p_case->Goal()->ToString().c_str());
	// p_case->TrialCount(p_case->TrialCount() + 1);
	// p_case->SuccessCount(p_case->SuccessCount() + (p_succeeded ? 1 : 0));

	// LogInfo("Revising case: %s, trial-count=%d, success-count=%d",
	// p_case->Goal()->ToString().c_str(), p_case->TrialCount(), p_case->SuccessCount());
}