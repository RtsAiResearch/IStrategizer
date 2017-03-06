#include "CaseBaseEx.h"
#include "GoalFactory.h"
#include "ObjectFactory.h"

using namespace IStrategizer;
using namespace std;
using namespace Serialization;

DECL_SERIALIZABLE(CaseEx);
DECL_SERIALIZABLE_NAMED(OlcbpPlan, "AdjListDigraph(PlanStepEx*)");
DECL_SERIALIZABLE(CaseBaseEx);

CaseEx* CaseBaseEx::NewCase(_In_ GoalType caseGoal)
{
	GoalEx* pNewGoal = g_GoalFactory.GetGoal(caseGoal, false);
	CaseEx* pNewCase = new CaseEx(new OlcbpPlan, pNewGoal, nullptr, 1, 1);
	CaseContainer.insert(pNewCase);

	return pNewCase;
}
//////////////////////////////////////////////////////////////////////////
void CaseBaseEx::Delete(CaseEx* pCase)
{
	CaseContainer.erase(pCase);
	SAFE_DELETE(pCase);
}
//////////////////////////////////////////////////////////////////////////
void CaseBaseEx::DeleteAll()
{
	for (SSet<CaseEx*>::iterator itr = CaseContainer.begin(); itr != CaseContainer.end();)
	{
		CaseEx* pCase = (*itr);
		CaseContainer.erase(itr++);
		SAFE_DELETE(pCase);
	}
}
//////////////////////////////////////////////////////////////////////////
void CaseBaseEx::DeleteAllGeneratedCases()
{
	for (SSet<CaseEx*>::iterator itr = CaseContainer.begin(); itr != CaseContainer.end();)
	{
		CaseEx* pCase = (*itr);
		if (pCase->Generated())
		{
			CaseContainer.erase(itr++);
			SAFE_DELETE(pCase);
		}
		else
			++itr;
	}
}
//////////////////////////////////////////////////////////////////////////
CaseEx* CaseBaseEx::FindByName(const std::string caseName)
{
    for (auto pCase : CaseContainer)
    {
        if (_stricmp(caseName.c_str(), pCase->Name().c_str()) == 0)
            return pCase;
    }

    return nullptr;
}