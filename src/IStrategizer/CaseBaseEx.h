///> [Serializable]
#ifndef CASEBASEEX_H
#define CASEBASEEX_H

#include "SVector.h"
#include "CaseEx.h"
#include "EngineObject.h"

namespace IStrategizer
{
    ///> class=CaseBaseEx
    class CaseBaseEx : public EngineObject
    {
		OBJECT_SERIALIZABLE(CaseBaseEx, &CaseContainer);
    public:
		void DeleteAllGeneratedCases();

        ///> type=vector(CaseEx*)
        Serialization::SVector<CaseEx*> CaseContainer;
    };
}

#endif // CASEBASEEX_H
