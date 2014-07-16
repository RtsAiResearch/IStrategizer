///> [Serializable]
#ifndef CASEBASEEX_H
#define CASEBASEEX_H

#include "SSet.h"
#include "CaseEx.h"
#include "EngineObject.h"

namespace IStrategizer
{
    ///> class=CaseBaseEx
    class CaseBaseEx : public EngineObject
    {
		OBJECT_SERIALIZABLE(CaseBaseEx, &CaseContainer);
    public:
		CaseEx* NewCase(_In_ GoalType caseGoal);
		void Delete(_In_ CaseEx* pCase);
		void DeleteAll();
		void DeleteAllGeneratedCases();

        ///> type=set(CaseEx*)
        Serialization::SSet<CaseEx*> CaseContainer;
    };
}

#endif // CASEBASEEX_H
