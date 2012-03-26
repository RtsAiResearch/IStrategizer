#ifndef PLANEXPANDEREX_H
#define PLANEXPANDEREX_H
#include <vector>
using namespace std;
class ActionEx;
class PlanTreeEx;
class CaseBasedReasonerEx;

class PlanExpanderEx
{
	CaseBasedReasonerEx* _caseBasedReasoner;
public:
	PlanExpanderEx();
	PlanExpanderEx(CaseBasedReasonerEx* p_caseBasedReasoner);
	vector<ActionEx*> GetReadyActions();
};

#endif