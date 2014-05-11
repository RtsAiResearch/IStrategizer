#ifndef REVISER_H
#include "Reviser.h"
#endif
#ifndef CASEEX_H
#include "CaseEx.h"
#endif
#include <cstdio>
#include "Logger.h"
#include "GoalEx.h"

using namespace IStrategizer;

void Reviser::Revise(CaseEx* pCase, bool succeeded)
{
    // Revise case usage
    LogWarning("Pretending to revise case %s, but case revision is not functional actually", pCase->Goal()->ToString().c_str());
    // pCase->TrialCount(pCase->TrialCount() + 1);
    // pCase->SuccessCount(pCase->SuccessCount() + (succeeded ? 1 : 0));

    LogInfo("Revising case: %s, trial-count=%d, success-count=%d", pCase->Goal()->ToString().c_str(), pCase->TrialCount(), pCase->SuccessCount());
}
