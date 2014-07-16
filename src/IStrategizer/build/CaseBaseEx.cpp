#include "CaseBaseEx.h"

using namespace IStrategizer;

void CaseBaseEx::DeleteAllGeneratedCases()
{
	for (auto itr = CaseContainer.begin(); itr != CaseContainer.end();)
	{
		auto& pCase = (*itr);
		if (pCase->Generated())
		{
		}
	}
}